#include "uci_engine.h"

bool UCIEngine::startEngine(bool debug, const std::string& enginePath) {
  debug = debug;
  if (pipe(engine_stdin) != 0 || pipe(engine_stdout) != 0) {
    std::cerr << "[GNUC] Failed to create pipes" << std::endl;
    return false;
  }

  engine_pid = fork();
  if (engine_pid == -1) {
    std::cerr << "[GNUC] Failed to fork process" << std::endl;
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
    std::cerr << "[GNUC] Failed to execute " << enginePath << std::endl;
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

    std::cout << "[GNUC] Engine started with PID: " << engine_pid << std::endl;
    return true;
  }
}

bool UCIEngine::sendCommand(const std::string& command, bool silent) {
  if (engine_stdin[1] == -1) return false;

  std::string full_command = command + "\n";
  int wbytes = write(engine_stdin[1], full_command.c_str(), full_command.length());
  fsync(engine_stdin[1]);

  if (!silent) std::cout << "[GNUC] Sent: " << command << " (w:" << wbytes << ")" << std::endl;
  return true;
}

std::vector<std::string> UCIEngine::getCommands() {
  std::lock_guard<std::mutex> lock(response_mutex);
  return commands;
}

std::string UCIEngine::getLastCommand() {
  std::lock_guard<std::mutex> lock(response_mutex);
  if (commands.empty()) return "";
  return commands.back();
}

void UCIEngine::clearCommands() {
  std::lock_guard<std::mutex> lock(response_mutex);
  commands.clear();
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
        std::cout << "[GNUC] Engine output closed" << std::endl;
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
    if (isCommandResponse(line)) {
      storeCommandResponse(line);
    }
    if (debug) std::cout << "[GNUC] " << line << std::endl;
  }
}

bool UCIEngine::isCommandResponse(const std::string& response) {
  // Define what constitutes an "important" response
  return (response.find("bestmove") != std::string::npos ||
      response.find("uciok") != std::string::npos ||
      response.find("readyok") != std::string::npos ||
      response.find("info depth") != std::string::npos ||
      response.find("score") != std::string::npos ||
      response.find("id name") != std::string::npos ||
      response.find("id author") != std::string::npos);
}

void UCIEngine::storeCommandResponse(const std::string& response) {
  std::lock_guard<std::mutex> lock(response_mutex);
  commands.push_back(response);

  // Keep only the last MAX_RESPONSES
  if (commands.size() > MAX_RESPONSES) {
    commands.erase(commands.begin());
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

void UCIEngine::setDifficult(int difficult) {
  this->difficult = difficult;
}

void UCIEngine::setMoveTime(uint32_t move_time) {
  this->move_time = move_time;
}

// Helper method to wait for specific response
bool UCIEngine::waitForResponse(const std::string& target, int timeout_ms) {
  auto start = std::chrono::steady_clock::now();

  while (std::chrono::steady_clock::now() - start < 
      std::chrono::milliseconds(timeout_ms)) {

    auto responses = getCommands();
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

void UCIEngine::addMoveToHistory(const std::string& move) {
  moves_history = moves_history + " " + move;
}

void UCIEngine::newGame() {
  moves_history.clear();
  sendCommand("ucinewgame");
}

void UCIEngine::setFenInitBoard(const std::string& fen) {
}

void UCIEngine::searchWithDepthAndTimeout(int depth, int max_time_ms) {
    std::atomic<bool> search_completed{false};
    std::string best_move;
    
    // Start search thread
    std::thread search_thread([&]() {
        sendCommand("go depth " + std::to_string(depth), !debug);
        // Wait for bestmove and set search_completed = true when done
    });
    
    // Start timeout thread
    std::thread timeout_thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(max_time_ms));
        if (!search_completed) {
            sendCommand("stop");
        }
    });
    
    search_thread.join();
    timeout_thread.join();
}

std::string UCIEngine::sendMove(const std::string& move) {
  std::string moves = "";
  if (move.size() > 4) {
    moves = "position fen " + move;
  }else{
    moves_history = moves_history + " " + move;
    moves = "position startpos moves" + moves_history;
  }
  sendCommand(moves, !debug);
  // sendCommand("go movetime 3000", debug);
  searchWithDepthAndTimeout(difficult, move_time * 1000);
  // Wait for bestmove asynchronously
  if (waitForResponse("bestmove", move_time * 1000 * 10)) {
    std::string lastMove = getLastCommand();
    commands.clear();
    std::string response = lastMove.substr(9, 4);
    return response;
  }
  else
    return "";
}


