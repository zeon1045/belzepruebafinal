#pragma once
#include <windows.h>
#include <string>
#include <functional>
#include <memory>
#include "driver_communicator.h"

using LogCallback = std::function<void(const std::string&, int)>;

class InjectionEngine {
private:
    DWORD targetProcessId = 0;
    std::unique_ptr<DriverCommunicator> m_driver;
    LogCallback logger;
    std::string m_serviceName;
    std::string m_driverPath;

public:
    InjectionEngine(LogCallback logCallback);
    ~InjectionEngine();

    bool LoadDriver(const std::string& serviceName, const std::string& driverPath);
    void UnloadDriver();
    bool AttachToProcess(DWORD pid);
    void DetachFromProcess();

    template<typename T>
    T ReadMemory(uintptr_t address) {
        T buffer{};
        if (m_driver && m_driver->IsLoaded() && targetProcessId != 0) {
            if (m_driver->ReadProcessMemory(targetProcessId, address, &buffer, sizeof(T))) {
                logger("Lectura OK en 0x" + std::to_string(address), 1);
                return buffer;
            }
        }
        logger("Fallo al leer memoria en 0x" + std::to_string(address), 2);
        return buffer;
    }

    template<typename T>
    bool WriteMemory(uintptr_t address, T value) {
        if (m_driver && m_driver->IsLoaded() && targetProcessId != 0) {
            if (m_driver->WriteMemory(targetProcessId, address, &value, sizeof(T))) {
                logger("Escritura OK en 0x" + std::to_string(address), 1);
                return true;
            }
        }
        logger("Fallo al escribir memoria en 0x" + std::to_string(address), 2);
        return false;
    }
};