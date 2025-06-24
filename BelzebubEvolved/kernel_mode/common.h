#pragma once

// Esta directiva nos permite usar el mismo header en kernel y user mode.
#ifdef _KERNEL_MODE
    #include <ntdef.h>
#else
    #include <windows.h>
#endif

// Comandos (IOCTLs) que nuestra aplicación puede enviar al driver.
#define IOCTL_READ_PROCESS_MEMORY   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_PROCESS_MEMORY  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Estructura de datos para las peticiones.
// Usamos tipos de tamaño fijo para asegurar compatibilidad entre 32/64 bits.
typedef struct _KERNEL_MEMORY_REQUEST {
    ULONG64 ProcessId;
    ULONGLONG Address;
    ULONGLONG Size;
    ULONGLONG BufferAddress; // Dirección del buffer en la aplicación user-mode.
} KERNEL_MEMORY_REQUEST, *PKERNEL_MEMORY_REQUEST;