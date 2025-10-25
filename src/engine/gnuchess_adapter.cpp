#include "gnuchess_adapter.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

GNUChessAdapter::GNUChessAdapter() : engineProcess(-1), inputPipe(-1), outputPipe(-1) {}

GNUChessAdapter::~GNUChessAdapter() {
    shutdown();
}

bool GNUChessAdapter::initialize() {
    int inputPipeFd[2];
    int outputPipeFd[2];
    
    // Create pipes for communication
    if (pipe(inputPipeFd) == -1 || pipe(outputPipeFd) == -1) {
        std::cerr << "Failed to create pipes for GNUChess" << std::endl;
        return false;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process for GNUChess" << std::endl;
        close(inputPipeFd[0]);
        close(inputPipeFd[1]);
        close(outputPipeFd[0]);
        close(outputPipeFd[1]);
        return false;
    }
    
    if (pid == 0) {
        // Child process - GNUChess
        close(inputPipeFd[1]);  // Close write end of input pipe
        close(outputPipeFd[0]); // Close read end of output pipe
        
        // Redirect stdin and stdout
        dup2(inputPipeFd[0], STDIN_FILENO);
        dup2(outputPipeFd[1], STDOUT_FILENO);
        
        // Close unused file descriptors
        close(inputPipeFd[0]);
        close(outputPipeFd[1]);
        
        // Execute GNUChess
        execl("/usr/games/gnuchess", "nice -n -10 gnuchess -q --memory 8", nullptr);
        
        // If we get here, exec failed
        std::cerr << "Failed to execute GNUChess" << std::endl;
        exit(1);
    } else {
        // Parent process
        close(inputPipeFd[0]);  // Close read end of input pipe
        close(outputPipeFd[1]); // Close write end of output pipe
        
        inputPipe = inputPipeFd[1];
        outputPipe = outputPipeFd[0];
        engineProcess = pid;
        
        // Set pipes to non-blocking
        fcntl(outputPipe, F_SETFL, O_NONBLOCK);
        
        // Wait for GNUChess to initialize
        usleep(5000000); // 5000ms
        
        // Send initialization commands
        sendCommand("xboard\n");
        sendCommand("protover 2\n");
        
        std::cout << "GNUChess engine initialized successfully" << std::endl;
        return true;
    }
}

void GNUChessAdapter::shutdown() {
    if (engineProcess != -1) {
        sendCommand("quit\n");

        usleep(500000); // 500ms
        
        // Wait for process to terminate
        int status;
        waitpid(engineProcess, &status, 0);
        
        close(inputPipe);
        close(outputPipe);
        
        engineProcess = -1;
        inputPipe = -1;
        outputPipe = -1;
    }
}

void GNUChessAdapter::newGame() {
    sendCommand("new\n");
    sendCommand("force\n");
    clearOutput();
}

void GNUChessAdapter::setPosition(const std::string& fen) {
    if (fen.empty() || fen == "startpos") {
        sendCommand("new\n");
    } else {
        sendCommand("setboard " + fen + "\n");
    }
    clearOutput();
}

void GNUChessAdapter::makeMove(const std::string& move) {
    sendCommand(move + "\n");
    clearOutput();
}

std::string GNUChessAdapter::getBestMove(int thinkTimeMs) {
    sendCommand("go\n");
    
    // Wait for engine to think
    usleep(thinkTimeMs * 1000);
    
    std::string response = readOutput();
    
    // Parse the response to extract the best move
    // GNUChess typically responds with "move e2e4" or similar
    size_t movePos = response.find("move ");
    if (movePos != std::string::npos) {
        size_t start = movePos + 5;
        size_t end = response.find('\n', start);
        if (end == std::string::npos) end = response.length();
        
        std::string move = response.substr(start, end - start);
        // Remove any extra whitespace
        move.erase(move.find_last_not_of(" \n\r\t") + 1);
        return move;
    }
    
    return "";
}

std::vector<std::string> GNUChessAdapter::getLegalMoves() {
    // This is a simplified implementation
    // In a real implementation, you'd parse the legal moves from engine output
    sendCommand("legal\n");
    std::string response = readOutput();
    
    std::vector<std::string> moves;
    std::istringstream iss(response);
    std::string line;
    
    while (std::getline(iss, line)) {
        if (line.length() >= 4 && line.find("legal") == std::string::npos) {
            moves.push_back(line);
        }
    }
    
    return moves;
}

std::string GNUChessAdapter::getBoardState() {
    sendCommand("d\n"); // Display board command
    return readOutput();
}

std::string GNUChessAdapter::getName() const {
    return "GNU Chess";
}

std::string GNUChessAdapter::getVersion() const {
    return "6.2.7";
}

void GNUChessAdapter::sendCommand(const std::string& command) {
    if (inputPipe != -1) {
        write(inputPipe, command.c_str(), command.length());
    }
}

std::string GNUChessAdapter::readOutput() {
    std::string output;
    char buffer[256];
    
    // Read all available output
    while (true) {
        ssize_t bytesRead = read(outputPipe, buffer, sizeof(buffer) - 1);
        if (bytesRead <= 0) {
            break;
        }
        buffer[bytesRead] = '\0';
        output += buffer;
        
        // Small delay to allow more data to arrive
        usleep(10000); // 10ms
    }
    
    return output;
}

void GNUChessAdapter::clearOutput() {
    // Read and discard any pending output
    char buffer[256];
    while (read(outputPipe, buffer, sizeof(buffer)) > 0) {
        // Discard data
    }
}
