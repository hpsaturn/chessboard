#include "version.h"
#include <string>

// These are defined by CMake at build time
#ifndef PROJECT_VERSION
#define PROJECT_VERSION "v0.1.3"
#endif

#ifndef GIT_VERSION
#define GIT_VERSION "v0.1.3-unknown"
#endif

#ifndef BUILD_INFO
#define BUILD_INFO "v0.1.3-unknown"
#endif

std::string Version::getVersion() {
    return PROJECT_VERSION;
}

std::string Version::getGitVersion() {
    return GIT_VERSION;
}

std::string Version::getBuildInfo() {
    return BUILD_INFO;
}

bool Version::isBuildrootEnvironment() {
    // Check if we're running in a Buildroot environment
    // This can be detected by checking for Buildroot-specific environment variables
    // or by checking if the version string contains "buildroot"
    std::string version = getGitVersion();
    return version.find("buildroot") != std::string::npos;
}
