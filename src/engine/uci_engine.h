#ifndef UCI_ENGINE_H
#define UCI_ENGINE_H

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>
#include <functional>
#include <queue>
#include <condition_variable>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <algorithm>
#include <signal.h>

class UCIEngine {
public:
    // Callback types
    using ResponseCallback = std::function<void(const std::string& response)>;
    using MoveCallback = std::function<void(const std::string& move)>;
    using ErrorCallback = std::function<void(const std::string& error)>;

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
    bool debug;
    int difficult = 1;
    uint16_t move_time = 2;

    // Async command queue
    struct AsyncCommand {
        std::string command;
        ResponseCallback callback;
        std::string expected_response;
        int timeout_ms;
    };
    
    std::queue<AsyncCommand> command_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::unique_ptr<std::thread> command_thread;
    std::atomic<bool> command_thread_running;

    // Callbacks
    MoveCallback move_callback;
    ErrorCallback error_callback;

public:
    UCIEngine() : engine_pid(-1), is_running(false), command_thread_running(false) {
        engine_stdin[0] = engine_stdin[1] = -1;
        engine_stdout[0] = engine_stdout[1] = -1;
    }
    
    ~UCIEngine() {
        shutdown();
    }

    bool startEngine(bool debug = false, const std::string& enginePath = "/usr/games/gnuchess");
    
    // Synchronous methods
    bool sendCommand(const std::string& command, bool silent = true);
    std::vector<std::string> getCommands();
    std::string getLastCommand();
    void clearCommands();
    bool waitForResponse(const std::string& target, int timeout_ms = 5000);
    void searchWithDepthAndTimeout(int depth, int max_time_ms);
    std::string sendMove(const std::string& move);
    std::string getMovesHistory();
    void addMoveToHistory(const std::string& move);
    void newGame();
    void setFenInitBoard(const std::string& fen);
    void shutdown();
    void setDifficult(int difficult);
    void setMoveTime(uint32_t move_time);

    // Async method with callbacks
    void sendMoveAsync(const std::string& move, MoveCallback callback = nullptr);
    
    // Callback setters
    void setMoveCallback(MoveCallback callback);
    void setErrorCallback(ErrorCallback callback);

private:
    void observerLoop();
    void commandProcessorLoop();
    void processEngineOutput(const char* data, std::string& partial_line);
    bool isCommandResponse(const std::string& response);
    void storeCommandResponse(const std::string& response);
    void notifyResponse(const std::string& response);
    void notifyMove(const std::string& move);
    void notifyError(const std::string& error);
};

#endif // UCI_ENGINE_H
