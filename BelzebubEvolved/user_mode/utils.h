#pragma once
#include <string>
#include <vector>

// Funciones de utilidad del sistema
std::string GetLastErrorAsString();
bool FileExists(const std::string& filepath);
std::vector<std::string> SplitString(const std::string& str, char delimiter); 