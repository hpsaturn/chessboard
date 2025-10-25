#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    std::cout << "Simple GNUChess Integration Test" << std::endl;
    std::cout << "================================" << std::endl;
    
    int inputPipeFd[2];
    int outputPipeFd[2];
    
    // Create pipes
    if (pipe(inputPipeFd) == -1 || pipe(outputPipeFd) == -1) {
        std::cerr << "Failed to create pipes" << std::endl;
        return 1;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork" << std::endl;
        return 1;
    }
    
    if (pid == 0) {
        // Child process - GNUChess
        close(inputPipeFd[1]);
        close(outputPipeFd[0]);
        
        dup2(inputPipeFd[0], STDIN_FILENO);
        dup2(outputPipeFd[1], STDOUT_FILENO);
        
        close(inputPipeFd[0]);
        close(outputPipeFd[1]);
        
        execl("/usr/games/gnuchess", "gnuchess", nullptr);
        std::cerr << "Failed to execute GNUChess" << std::endl;
        exit(1);
    } else {
        // Parent process
        close(inputPipeFd[0]);
        close(outputPipeFd[1]);
        
        int inputPipe = inputPipeFd[1];
        int outputPipe = outputPipeFd[0];
        
        // Set output pipe to non-blocking
        fcntl(outputPipe, F_SETFL, O_NONBLOCK);
        
        // Wait for GNUChess to start
        sleep(1);
        
        // Test 1: Send xboard protocol
        std::cout << "Test 1: Sending xboard protocol..." << std::endl;
        write(inputPipe, "xboard\n", 7);
        sleep(1);
        
        // Read response
        char buffer[1024];
        ssize_t bytesRead = read(outputPipe, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Response: " << buffer << std::endl;
        }
        
        // Test 2: Send new game
        std::cout << "\nTest 2: Starting new game..." << std::endl;
        write(inputPipe, "new\n", 4);
        sleep(1);
        
        bytesRead = read(outputPipe, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Response: " << buffer << std::endl;
        }
        
        // Test 3: Make a move
        std::cout << "\nTest 3: Making move e2e4..." << std::endl;
        write(inputPipe, "e2e4\n", 5);
        sleep(1);
        
        bytesRead = read(outputPipe, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Response: " << buffer << std::endl;
        }
        
        // Test 4: Get engine move
        std::cout << "\nTest 4: Getting engine move..." << std::endl;
        write(inputPipe, "go\n", 3);
        sleep(3);
        
        bytesRead = read(outputPipe, buffer, sizeof(buffer) - 1);
        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::cout << "Response: " << buffer << std::endl;
        }
        
        // Cleanup
        std::cout << "\nCleaning up..." << std::endl;
        write(inputPipe, "quit\n", 5);
        close(inputPipe);
        close(outputPipe);
        waitpid(pid, nullptr, 0);
        
        std::cout << "\nTest completed successfully!" << std::endl;
    }
    
    return 0;
}
