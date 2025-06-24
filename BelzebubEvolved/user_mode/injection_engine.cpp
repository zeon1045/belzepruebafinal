#include "injection_engine.h"
#include "privilege_manager.h"
#include <iostream>

InjectionEngine::InjectionEngine(LogCallback logCallback) : logger(logCallback) {}

InjectionEngine::~InjectionEngine() {
    UnloadDriver();
}

bool InjectionEngine::LoadDriver(const std::string& serviceName, const std::string& driverPath) {
    logger("Intentando cargar driver '" + driverPath + "'...", 0);
    m_serviceName = serviceName;
    m_driverPath = driverPath;

    if (LoadDriverService(m_serviceName, m_driverPath)) {
        m_driver = std::make_unique<DriverCommunicator>();
        if (m_driver->IsLoaded()) {
            logger("Driver cargado y handle obtenido. Sistema listo.", 1);
            return true;
        } else {
            logger("Driver cargado como servicio, pero no se pudo obtener handle.", 2);
            UnloadDriverService(m_serviceName);
            return false;
        }
    }
    
    logger("Fallo al cargar el servicio del driver.", 2);
    return false;
}

void InjectionEngine::UnloadDriver() {
    if (m_driver) {
        m_driver.reset();
    }
    if (!m_serviceName.empty()) {
        UnloadDriverService(m_serviceName);
        logger("Driver descargado y comunicación cerrada.", 0);
        m_serviceName.clear();
    }
}

bool InjectionEngine::AttachToProcess(DWORD pid) {
    if (pid == 0) {
        logger("PID inválido.", 2);
        return false;
    }
    targetProcessId = pid;
    logger("Adjuntado al proceso con PID: " + std::to_string(pid), 1);
    return true;
}

void InjectionEngine::DetachFromProcess() {
    targetProcessId = 0;
    logger("Desadjuntado del proceso.", 0);
}