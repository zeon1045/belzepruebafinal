#pragma once
#include <windows.h>
#include <string>
#include "../../kernel_mode/common.h" // Usamos la ruta relativa

class DriverCommunicator {
private:
    HANDLE hDriver = INVALID_HANDLE_VALUE;

public:
    DriverCommunicator(const std::wstring& driverSymLink);
    ~DriverCommunicator();

    bool IsLoaded() const;
    bool ReadMemory(HANDLE pid, uintptr_t address, PVOID buffer, size_t size);
    bool WriteMemory(HANDLE pid, uintptr_t address, PVOID buffer, size_t size);
}; 