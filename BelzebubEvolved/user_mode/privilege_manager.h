#pragma once
#include <windows.h>
#include <string>

// Funciones originales para privilegios
bool SetPrivilege(const wchar_t* lpszPrivilege, bool bEnablePrivilege);
bool IsUserAdmin();

// Nuevas funciones para gestionar el servicio del driver
bool LoadDriverService(const std::string& serviceName, const std::string& driverPath);
bool UnloadDriverService(const std::string& serviceName);