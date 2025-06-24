#include "utils.h"
#include <windows.h>
#include <iostream>
#include <sstream>
#include <fstream>

std::string GetLastErrorAsString() {
    DWORD errorMessageID = GetLastError();
    if (errorMessageID == 0) {
        return "No error";
    }
    
    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    
    std::string message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

bool FileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::vector<std::string> SplitString(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
} 