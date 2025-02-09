#include "FileUtils.h"


std::string File_Utils::readFile(const char* filePath) {
    std::ifstream file(filePath);
    if (!file) 
        throw std::runtime_error("Unable to initialize file! File path was " + static_cast<std::string>(filePath));
    
    std::stringstream buffer;
    buffer << file.rdbuf();

    if (buffer.fail())
        throw std::runtime_error("Unable to read file! Last line read was: " + buffer.str());
    
    return buffer.str();
}
