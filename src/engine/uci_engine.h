#ifndef UCI_ENGINE_H
#define UCI_ENGINE_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <algorithm>
#include <signal.h>

class UCIEngine {
private:
    // Pipe file descriptors
    int engine_stdin[2];  // [0] read, [1] write
    int engine_stdout[2]; // [0] read, [1] write
    pid_t engine_pid;
    
    // Thread control
    std::atomic<bool> is_running;
    std::unique_ptr<std::thread> observer_thread;
    
    // Response storage
    std::vector<std::string> important_responses;
    std::mutex response_mutex;
    std::string moves_history = "";
    static const size_t MAX_RESPONSES = 50;

public:
    UCIEngine() : engine_pid(-1), is_running(false) {
        engine_stdin[0] = engine_stdin[1] = -1;
        engine_stdout[0] = engine_stdout[1] = -1;
    }
    
    ~UCIEngine() {
        shutdown();
    }

    bool startEngine(const std::string& enginePath = "/usr/games/gnuchess");
    void sendCommand(const std::string& command, bool silent = true);
    std::vector<std::string> getImportantResponses();
    std::string getLastImportantResponse();
    void clearResponses();
    void shutdown();
    bool waitForResponse(const std::string& target, int timeout_ms = 5000);

    std::string sendMove(const std::string& move);
    std::string getMovesHistory();
    void newGame();

private:
    void observerLoop();
    void processEngineOutput(const char* data, std::string& partial_line);
    bool isImportantResponse(const std::string& response);
    void storeImportantResponse(const std::string& response);
};

#endif // UCI_ENGINE_H
