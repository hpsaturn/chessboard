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
    std::vector<std::string> commands;
    std::mutex response_mutex;
    std::string moves_history = "";
    static const size_t MAX_RESPONSES = 50;
    bool debug = true;
    int difficult = 1;
    uint16_t move_time = 2;

public:
    UCIEngine(bool debug = false) : engine_pid(-1), is_running(false),debug(debug) {
        engine_stdin[0] = engine_stdin[1] = -1;
        engine_stdout[0] = engine_stdout[1] = -1;
    }
    
    ~UCIEngine() {
        shutdown();
    }

    bool startEngine(const std::string& enginePath = "/usr/games/gnuchess");
    void sendCommand(const std::string& command, bool silent = true);
    std::vector<std::string> getCommands();
    std::string getLastCommand();
    void clearCommands();
    bool waitForResponse(const std::string& target, int timeout_ms = 5000);
    void searchWithDepthAndTimeout(int depth, int max_time_ms);
    std::string sendMove(const std::string& move);
    std::string getMovesHistory();
    void addMoveToHistory(const std::string& move);
    void newGame();
    void shutdown();
    void setDifficult(int difficult);
    void setMoveTime(uint32_t move_time);

private:
    void observerLoop();
    void processEngineOutput(const char* data, std::string& partial_line);
    bool isCommandResponse(const std::string& response);
    void storeCommandResponse(const std::string& response);
};

#endif // UCI_ENGINE_H
