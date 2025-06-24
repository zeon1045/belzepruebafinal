#include "privilege_manager.h"
#include <iostream>
#include <windows.h>
#include <winsvc.h> // Necesario para Service Control Manager

// El código para SetPrivilege y IsUserAdmin se mantiene como en tu proyecto original.
// ... (pega aquí tu código funcional de SetPrivilege e IsUserAdmin) ...

// Implementación para las nuevas funciones de servicio.
bool LoadDriverService(const std::string& serviceName, const std::string& driverPath) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
    if (!hSCManager) {
        std::cerr << "Error: No se pudo abrir SCM: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE hService = CreateServiceA(
        hSCManager, serviceName.c_str(), serviceName.c_str(),
        SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        driverPath.c_str(), NULL, NULL, NULL, NULL, NULL);

    if (!hService) {
        if (GetLastError() == ERROR_SERVICE_EXISTS) {
            hService = OpenServiceA(hSCManager, serviceName.c_str(), SERVICE_START | SERVICE_STOP | DELETE);
        } else {
            std::cerr << "Error: No se pudo crear el servicio: " << GetLastError() << std::endl;
            CloseServiceHandle(hSCManager);
            return false;
        }
    }

    bool success = StartService(hService, 0, NULL);
    if (!success && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING) {
        std::cerr << "Error: No se pudo iniciar el servicio: " << GetLastError() << std::endl;
        DeleteService(hService); // Limpieza si no se pudo iniciar
        success = false;
    } else {
        success = true; // Éxito si se inicia o si ya estaba corriendo
    }

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return success;
}

bool UnloadDriverService(const std::string& serviceName) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!hSCManager) return false;

    SC_HANDLE hService = OpenServiceA(hSCManager, serviceName.c_str(), SERVICE_STOP | DELETE);
    if (!hService) {
        CloseServiceHandle(hSCManager);
        return true; // El servicio no existe, lo cual es un estado deseado.
    }

    SERVICE_STATUS status;
    ControlService(hService, SERVICE_CONTROL_STOP, &status);
    DeleteService(hService);

    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}