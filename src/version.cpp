#include "version.h"
#include <string>

// These will be replaced by CMake during build
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
