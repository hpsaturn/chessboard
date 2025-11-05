// Game State Manager for Chess Game
#ifndef GAME_STATE_MANAGER_H
#define GAME_STATE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <yaml-cpp/yaml.h>

class GameStateManager {
public:
    // Game State structure
    struct GameState {
        std::string date;     // Format: YYYYMMDD
        std::string fen;      // FEN notation
        std::string title;    // State title/description
    };

    GameStateManager();
    ~GameStateManager() = default;

    // Load all game states from file
    bool loadGameStates();
    
    // Save all game states to file
    bool saveGameStates();

    // Get all game states
    const std::vector<GameState>& getGameStates() const { return gameStates; }
    
    // Get game state by date
    GameState* getGameState(const std::string& date);
    
    // Add a new game state
    bool addGameState(const std::string& fen, const std::string& title);
    
    // Remove game state by date
    bool removeGameState(const std::string& date);
    
    // Update existing game state
    bool updateGameState(const std::string& date, const std::string& fen, const std::string& title);

    // Get states file path
    std::string getStatesPath() const { return statesPath; }

private:
    std::string statesPath;
    std::string configDir;
    std::vector<GameState> gameStates;
    
    // Ensure config directory exists
    bool ensureConfigDir();
    
    // Generate current date string (YYYYMMDD)
    std::string getCurrentDate() const;
    
    // Convert game states to YAML node
    YAML::Node statesToYaml() const;
    
    // Convert YAML node to game states
    bool yamlToStates(const YAML::Node& node);
};

#endif // GAME_STATE_MANAGER_H
