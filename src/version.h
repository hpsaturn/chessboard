#ifndef VERSION_H
#define VERSION_H

#include <string>

class Version {
public:
    static std::string getVersion();
    static std::string getGitVersion();
    static std::string getBuildInfo();
    
private:
    static std::string executeGitCommand(const std::string& command);
};

#endif
