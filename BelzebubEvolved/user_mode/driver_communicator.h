#pragma once
#include <windows.h>
#include <string>
#include "common.h" // Incluye la ruta corregida por CMake

class DriverCommunicator {
private:
    HANDLE hDriver;

public:
    DriverCommunicator(const std::wstring& driverSymLink = L"\\\\.\\Belzebub");
    ~DriverCommunicator();

    bool IsLoaded() const;
    bool ReadMemory(DWORD pid, uintptr_t address, PVOID buffer, size_t size);
    bool WriteMemory(DWORD pid, uintptr_t address, PVOID buffer, size_t size);
};