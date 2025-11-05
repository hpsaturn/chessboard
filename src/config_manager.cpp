#include "config_manager.h"
#include <iostream>
#include <filesystem>
#include <fstream>

ConfigManager::ConfigManager() {
    // Get user home directory
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        std::cerr << "Error: HOME environment variable not set" << std::endl;
        return;
    }
    
    // Set config directory and file path
    configDir = std::string(homeDir) + "/.chessboard";
    configPath = configDir + "/config.yml";
    
    // Ensure config directory exists
    if (!ensureConfigDir()) {
        std::cerr << "Error: Failed to create config directory: " << configDir << std::endl;
    }
}

bool ConfigManager::ensureConfigDir() {
    try {
        if (!std::filesystem::exists(configDir)) {
            return std::filesystem::create_directories(configDir);
        }
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::loadSettings(Settings& settings) {
    try {
        if (!std::filesystem::exists(configPath)) {
            std::cout << "Config file not found, using default settings" << std::endl;
            return false; // File doesn't exist, use defaults
        }
        
        YAML::Node config = YAML::LoadFile(configPath);
        settings = yamlToSettings(config);
        
        std::cout << "Settings loaded from: " << configPath << std::endl;
        return true;
        
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parsing error: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error loading settings: " << e.what() << std::endl;
        return false;
    }
}

bool ConfigManager::saveSettings(const Settings& settings) {
    try {
        YAML::Node config = settingsToYaml(settings);
        
        std::ofstream file(configPath);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open config file for writing: " << configPath << std::endl;
            return false;
        }
        
        file << config;
        file.close();
        
        std::cout << "Settings saved to: " << configPath << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error saving settings: " << e.what() << std::endl;
        return false;
    }
}

YAML::Node ConfigManager::settingsToYaml(const Settings& settings) {
    YAML::Node node;
    
    node["depth_difficulty"] = settings.depthDifficulty;
    node["max_time_per_move"] = settings.maxTimePerMove;
    node["match_time"] = settings.matchTime;
    node["sound_enabled"] = settings.soundEnabled;
    
    return node;
}

ConfigManager::Settings ConfigManager::yamlToSettings(const YAML::Node& node) {
    Settings settings;
    
    if (node["depth_difficulty"]) {
        settings.depthDifficulty = node["depth_difficulty"].as<int>();
    }
    
    if (node["max_time_per_move"]) {
        settings.maxTimePerMove = node["max_time_per_move"].as<int>();
    }
    
    if (node["match_time"]) {
        settings.matchTime = node["match_time"].as<int>();
    }
    
    if (node["sound_enabled"]) {
        settings.soundEnabled = node["sound_enabled"].as<bool>();
    }
    
    return settings;
}
