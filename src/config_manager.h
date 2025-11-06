// Configuration Manager for Chess Game
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <string>
#include <yaml-cpp/yaml.h>

class ConfigManager {
public:
    // Settings structure (mirrors SettingsModal::Settings)
    struct Settings {
        int depthDifficulty = 1;      // 1-10
        int maxTimePerMove = 2;       // seconds (0-300)
        int matchTime = 10;           // minutes (0-60)
        bool soundEnabled = false;
    };

    ConfigManager();
    ~ConfigManager() = default;

    // Load settings from config file
    bool loadSettings(Settings& settings);
    
    // Save settings to config file
    bool saveSettings(const Settings& settings);

    // Get config file path
    std::string getConfigPath() const { return configPath; }

private:
    std::string configPath;
    std::string configDir;
    
    // Ensure config directory exists
    bool ensureConfigDir();
    
    // Convert settings to YAML node
    YAML::Node settingsToYaml(const Settings& settings);
    
    // Convert YAML node to settings
    Settings yamlToSettings(const YAML::Node& node);
};

#endif // CONFIG_MANAGER_H
