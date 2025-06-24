#include "driver_communicator.h"
#include <vector>
#include <iostream>

DriverCommunicator::DriverCommunicator(const std::wstring& driverSymLink) {
    hDriver = CreateFileW(driverSymLink.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (!IsLoaded()) {
        std::wcerr << L"No se pudo obtener un handle al driver: " << GetLastError() << std::endl;
    }
}

DriverCommunicator::~DriverCommunicator() {
    if (IsLoaded()) {
        CloseHandle(hDriver);
    }
}

bool DriverCommunicator::IsLoaded() const {
    return hDriver != INVALID_HANDLE_VALUE;
}

bool DriverCommunicator::ReadMemory(HANDLE pid, uintptr_t address, PVOID buffer, size_t size) {
    if (!IsLoaded()) return false;
    
    KERNEL_MEMORY_REQUEST request;
    request.ProcessId = pid;
    request.Address = address;
    request.Size = size;
    request.Buffer = (ULONGLONG)buffer; // Pasamos la dirección del buffer

    DWORD bytesReturned;
    return DeviceIoControl(hDriver, IOCTL_READ_PROCESS_MEMORY, &request, sizeof(request), &request, sizeof(request), &bytesReturned, NULL);
}

bool DriverCommunicator::WriteMemory(HANDLE pid, uintptr_t address, PVOID buffer, size_t size) {
    if (!IsLoaded()) return false;
    
    KERNEL_MEMORY_REQUEST request;
    request.ProcessId = pid;
    request.Address = address;
    request.Size = size;
    request.Buffer = (ULONGLONG)buffer; // Pasamos la dirección del buffer

    DWORD bytesReturned;
    return DeviceIoControl(hDriver, IOCTL_WRITE_PROCESS_MEMORY, &request, sizeof(request), nullptr, 0, &bytesReturned, NULL);
} 