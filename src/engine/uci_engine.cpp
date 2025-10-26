#include "uci_engine.h"

bool UCIEngine::startEngine(const std::string& enginePath) {
  if (pipe(engine_stdin) != 0 || pipe(engine_stdout) != 0) {
    std::cerr << "Failed to create pipes" << std::endl;
    return false;
  }

  engine_pid = fork();
  if (engine_pid == -1) {
    std::cerr << "Failed to fork process" << std::endl;
    return false;
  }

  if (engine_pid == 0) {
    // Child process - GNU Chess
    close(engine_stdin[1]);  // Close write end of stdin
    close(engine_stdout[0]); // Close read end of stdout

    // Redirect stdin/stdout to our pipes
    dup2(engine_stdin[0], STDIN_FILENO);
    dup2(engine_stdout[1], STDOUT_FILENO);

    // Close the original descriptors
    close(engine_stdin[0]);
    close(engine_stdout[1]);

    // Execute GNU Chess with UCI mode
    execlp(enginePath.c_str(), enginePath.c_str(), "--uci", nullptr);

    // If we get here, exec failed
    std::cerr << "Failed to execute " << enginePath << std::endl;
    exit(1);
  } else {
    // Parent process - our connector
    close(engine_stdin[0]);  // Close read end of stdin
    close(engine_stdout[1]); // Close write end of stdout

    // Set non-blocking reads on stdout
    fcntl(engine_stdout[0], F_SETFL, O_NONBLOCK);

    // Start observer thread
    is_running = true;
    observer_thread = std::make_unique<std::thread>(&UCIEngine::observerLoop, this);

    std::cout << "Engine started with PID: " << engine_pid << std::endl;
    return true;
  }
}

void UCIEngine::sendCommand(const std::string& command, bool silent) {
  if (engine_stdin[1] == -1) return;

  std::string full_command = command + "\n";
  write(engine_stdin[1], full_command.c_str(), full_command.length());
  fsync(engine_stdin[1]);

  if (!silent) std::cout << "Sent: " << command << std::endl;
}

std::vector<std::string> UCIEngine::getImportantResponses() {
  std::lock_guard<std::mutex> lock(response_mutex);
  return important_responses;
}

std::string UCIEngine::getLastImportantResponse() {
  std::lock_guard<std::mutex> lock(response_mutex);
  if (important_responses.empty()) return "";
  return important_responses.back();
}

void UCIEngine::clearResponses() {
  std::lock_guard<std::mutex> lock(response_mutex);
  important_responses.clear();
}

void UCIEngine::observerLoop() {
  char buffer[4096];
  std::string partial_line;

  while (is_running) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(engine_stdout[0], &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100ms

    int result = select(engine_stdout[0] + 1, &read_fds, nullptr, nullptr, &timeout);

    if (result > 0 && FD_ISSET(engine_stdout[0], &read_fds)) {
      ssize_t bytes_read = read(engine_stdout[0], buffer, sizeof(buffer) - 1);

      if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        processEngineOutput(buffer, partial_line);
      } else if (bytes_read == 0) {
        // EOF - engine closed output
        std::cout << "Engine output closed" << std::endl;
        break;
      }
    }

    // Small sleep to prevent CPU spinning
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
}

void UCIEngine::processEngineOutput(const char* data, std::string& partial_line) {
  std::string output = partial_line + data;
  partial_line.clear();

  std::vector<std::string> lines;
  size_t start = 0;
  size_t end = output.find('\n');

  while (end != std::string::npos) {
    std::string line = output.substr(start, end - start);
    // Remove carriage return if present
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    lines.push_back(line);
    start = end + 1;
    end = output.find('\n', start);
  }

  // Save incomplete line for next read
  if (start < output.length()) {
    partial_line = output.substr(start);
  }

  // Process complete lines
  for (const auto& line : lines) {
    if (isImportantResponse(line)) {
      storeImportantResponse(line);
    }
    std::cout << "Engine: " << line << std::endl;
  }
}

bool UCIEngine::isImportantResponse(const std::string& response) {
  // Define what constitutes an "important" response
  return (response.find("bestmove") != std::string::npos ||
      response.find("uciok") != std::string::npos ||
      response.find("readyok") != std::string::npos ||
      response.find("info depth") != std::string::npos ||
      response.find("score") != std::string::npos ||
      response.find("id name") != std::string::npos ||
      response.find("id author") != std::string::npos);
}

void UCIEngine::storeImportantResponse(const std::string& response) {
  std::lock_guard<std::mutex> lock(response_mutex);
  important_responses.push_back(response);

  // Keep only the last MAX_RESPONSES
  if (important_responses.size() > MAX_RESPONSES) {
    important_responses.erase(important_responses.begin());
  }
}

void UCIEngine::shutdown() {
  is_running = false;

  // Stop observer thread
  if (observer_thread && observer_thread->joinable()) {
    observer_thread->join();
  }

  // Send quit command and close pipes
  if (engine_stdin[1] != -1) {
    sendCommand("quit");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    close(engine_stdin[1]);
    engine_stdin[1] = -1;
  }

  if (engine_stdout[0] != -1) {
    close(engine_stdout[0]);
    engine_stdout[0] = -1;
  }

  // Kill engine if still running
  if (engine_pid > 0) {
    kill(engine_pid, SIGTERM);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    kill(engine_pid, SIGKILL);

    waitpid(engine_pid, nullptr, 0);
    engine_pid = -1;
  }
}

// Helper method to wait for specific response
bool UCIEngine::waitForResponse(const std::string& target, int timeout_ms) {
  auto start = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() - start < 
      std::chrono::milliseconds(timeout_ms)) {

    auto responses = getImportantResponses();
    for (const auto& response : responses) {
      if (response.find(target) != std::string::npos) {
        return true;
      }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  return false;
}

std::string UCIEngine::getMovesHistory() { 
  return moves_history; 
}

void UCIEngine::newGame() {
  sendCommand("ucinewgame");
}

std::string UCIEngine::sendMove(const std::string& move) {
  moves_history = moves_history + " " + move;
  std::string moves = "position startpos moves" + moves_history;
  sendCommand(moves);
  sendCommand("go movetime 3000");
  // Wait for bestmove asynchronously
  if (waitForResponse("bestmove", 20000)) {
    std::string lastMove = getLastImportantResponse();
    important_responses.clear();
    std::string response = lastMove.substr(9, 4);
    return response;
  }
  else
    return "";
}
