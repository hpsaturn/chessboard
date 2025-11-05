#include "game_state_manager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <ctime>
#include <algorithm>

GameStateManager::GameStateManager() {
    // Get user home directory
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "[STATE] Error: HOME environment variable not set" << std::endl;
        return;
    }
    
    // Set config directory and file path
    configDir = std::string(homeDir) + "/.chessboard";
    statesPath = configDir + "/states.yml";
    
    // Ensure config directory exists
    if (!ensureConfigDir()) {
        std::cerr << "[STATE] Error: Failed to create config directory: " << configDir << std::endl;
    }
}

bool GameStateManager::ensureConfigDir() {
    try {
        if (!std::filesystem::exists(configDir)) {
            return std::filesystem::create_directories(configDir);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "[STATE] Filesystem error: " << e.what() << std::endl;
        return false;
    }
}

bool GameStateManager::loadGameStates() {
    try {
        if (!std::filesystem::exists(statesPath)) {
            std::cout << "[STATE] States file not found, starting with empty states" << std::endl;
            gameStates.clear();
            return true; // File doesn't exist, start with empty states
        }
        
        YAML::Node statesYaml = YAML::LoadFile(statesPath);
        bool success = yamlToStates(statesYaml);
        
        if (success) {
            std::cout << "[STATE] Loaded " << gameStates.size() << " game states from: " << statesPath << std::endl;
        }
        
        return success;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "[STATE] YAML parsing error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[STATE] Error loading game states: " << e.what() << std::endl;
        return false;
    }
}

bool GameStateManager::saveGameStates() {
    try {
        YAML::Node statesYaml = statesToYaml();
        
        std::ofstream file(statesPath);
        if (!file.is_open()) {
            std::cerr << "[STATE] Error: Could not open states file for writing: " << statesPath << std::endl;
            return false;
        }
        
        file << statesYaml;
        file.close();
        
        std::cout << "[STATE] Saved " << gameStates.size() << " game states to: " << statesPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[STATE] Error saving game states: " << e.what() << std::endl;
        return false;
    }
}

GameStateManager::GameState* GameStateManager::getGameState(const std::string& date) {
    auto it = std::find_if(gameStates.begin(), gameStates.end(),
                          [&date](const GameState& state) { return state.date == date; });
    
    if (it != gameStates.end()) {
        return &(*it);
    }
    return nullptr;
}

bool GameStateManager::addGameState(const std::string& fen, const std::string& title) {
    std::string currentDate = getCurrentDate();
    
    // Check if state with this date already exists
    if (getGameState(currentDate)) {
        std::cerr << "[STATE] Error: Game state with date " << currentDate << " already exists" << std::endl;
        return false;
    }
    
    GameState newState;
    newState.date = currentDate;
    newState.fen = fen;
    newState.title = title;
    
    gameStates.push_back(newState);
    
    std::cout << "[STATE] Added new game state: " << title << " (" << currentDate << ")" << std::endl;
    return saveGameStates();
}

bool GameStateManager::removeGameState(const std::string& date) {
    auto it = std::find_if(gameStates.begin(), gameStates.end(),
                          [&date](const GameState& state) { return state.date == date; });
    
    if (it != gameStates.end()) {
        std::string title = it->title;
        gameStates.erase(it);
        std::cout << "[STATE] Removed game state: " << title << " (" << date << ")" << std::endl;
        return saveGameStates();
    }
    
    std::cerr << "[STATE] Error: Game state with date " << date << " not found" << std::endl;
    return false;
}

bool GameStateManager::updateGameState(const std::string& date, const std::string& fen, const std::string& title) {
    GameState* state = getGameState(date);
    if (state) {
        state->fen = fen;
        state->title = title;
        std::cout << "[STATE] Updated game state: " << title << " (" << date << ")" << std::endl;
        return saveGameStates();
    }
    
    std::cerr << "[STATE] Error: Game state with date " << date << " not found" << std::endl;
    return false;
}

std::string GameStateManager::getCurrentDate() const {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    
    char buffer[16]; // YYYYMMDD + null terminator
    std::strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", now);
    
    return std::string(buffer);
}

YAML::Node GameStateManager::statesToYaml() const {
    YAML::Node root;
    
    for (const auto& state : gameStates) {
        YAML::Node stateNode;
        stateNode["fen"] = state.fen;
        stateNode["title"] = state.title;
        
        root[state.date] = stateNode;
    }
    
    return root;
}

bool GameStateManager::yamlToStates(const YAML::Node& node) {
    gameStates.clear();
    
    if (!node.IsMap()) {
        std::cerr << "[STATE] Error: Invalid YAML structure - expected map" << std::endl;
        return false;
    }
    
    for (const auto& entry : node) {
        std::string date = entry.first.as<std::string>();
        YAML::Node stateNode = entry.second;
        
        if (stateNode["fen"] && stateNode["title"]) {
            GameState state;
            state.date = date;
            state.fen = stateNode["fen"].as<std::string>();
            state.title = stateNode["title"].as<std::string>();
            
            gameStates.push_back(state);
        } else {
            std::cerr << "[STATE] Warning: Invalid state entry for date " << date << std::endl;
        }
    }
    
    return true;
}
