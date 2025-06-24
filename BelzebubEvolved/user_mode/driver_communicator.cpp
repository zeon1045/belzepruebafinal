#include "driver_communicator.h"
#include <vector>

DriverCommunicator::DriverCommunicator(const std::wstring& driverSymLink) {
    hDriver = CreateFileW(driverSymLink.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}

DriverCommunicator::~DriverCommunicator() {
    if (IsLoaded()) {
        CloseHandle(hDriver);
    }
}

bool DriverCommunicator::IsLoaded() const {
    return hDriver != INVALID_HANDLE_VALUE;
}

bool DriverCommunicator::ReadMemory(DWORD pid, uintptr_t address, PVOID buffer, size_t size) {
    if (!IsLoaded()) return false;

    KERNEL_MEMORY_REQUEST request{};
    request.ProcessId = pid;
    request.Address = address;
    request.Size = size;
    request.BufferAddress = (ULONGLONG)buffer;

    DWORD bytesReturned = 0;
    return DeviceIoControl(hDriver, IOCTL_READ_PROCESS_MEMORY, &request, sizeof(request), &request, sizeof(request), &bytesReturned, nullptr);
}

bool DriverCommunicator::WriteMemory(DWORD pid, uintptr_t address, PVOID buffer, size_t size) {
    if (!IsLoaded()) return false;

    std::vector<BYTE> requestBuffer(sizeof(KERNEL_MEMORY_REQUEST));
    PKERNEL_MEMORY_REQUEST pRequest = (PKERNEL_MEMORY_REQUEST)requestBuffer.data();
    
    pRequest->ProcessId = pid;
    pRequest->Address = address;
    pRequest->Size = size;
    pRequest->BufferAddress = (ULONGLONG)buffer;

    DWORD bytesReturned = 0;
    return DeviceIoControl(hDriver, IOCTL_WRITE_PROCESS_MEMORY, pRequest, (DWORD)requestBuffer.size(), nullptr, 0, &bytesReturned, nullptr);
}