#include "version.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>

std::string Version::executeGitCommand(const std::string& command) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
    
    if (!pipe) {
        return "unknown";
    }
    
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    
    // Remove trailing newline
    if (!result.empty() && result[result.length()-1] == '\n') {
        result.erase(result.length()-1);
    }
    
    return result;
}

std::string Version::getVersion() {
    // Try to get the latest tag
    std::string latestTag = executeGitCommand("git describe --tags --abbrev=0 2>/dev/null");
    
    if (latestTag != "unknown" && !latestTag.empty()) {
        return latestTag;
    }
    
    // Fallback: try to get version from git describe
    std::string describe = executeGitCommand("git describe --tags --always 2>/dev/null");
    
    if (describe != "unknown" && !describe.empty()) {
        return describe;
    }
    
    // Final fallback
    return "v0.1.3";
}

std::string Version::getGitVersion() {
    // Get detailed version info with commit hash
    std::string version = executeGitCommand("git describe --tags --always --dirty 2>/dev/null");
    
    if (version != "unknown" && !version.empty()) {
        return version;
    }
    
    // Fallback to simple version
    return getVersion();
}

std::string Version::getBuildInfo() {
    std::stringstream info;
    
    // Get version
    info << getVersion();
    
    // Get commit hash
    std::string commitHash = executeGitCommand("git rev-parse --short HEAD 2>/dev/null");
    if (commitHash != "unknown" && !commitHash.empty()) {
        info << " (" << commitHash << ")";
    }
    
    // Check if there are uncommitted changes
    std::string status = executeGitCommand("git status --porcelain 2>/dev/null");
    if (!status.empty()) {
        info << " [dirty]";
    }
    
    return info.str();
}
