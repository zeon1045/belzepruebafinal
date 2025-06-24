#pragma once

// Este IOCTL (código de control de E/S) será nuestro canal de comunicación.
// Define un identificador único para cada operación que el driver puede realizar.
#define IOCTL_READ_PROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x900, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_PROCESS_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_ANY_ACCESS)

// Estructura para enviar peticiones de lectura/escritura al driver.
// Usamos ULONGLONG para direcciones de 64 bits.
typedef struct _KERNEL_MEMORY_REQUEST {
    HANDLE ProcessId;     // PID del proceso objetivo.
    ULONGLONG Address;    // Dirección de memoria a leer/escribir.
    ULONGLONG Size;       // Tamaño de los datos.
    ULONGLONG Buffer;     // Puntero al buffer (en user-mode) con los datos.
} KERNEL_MEMORY_REQUEST, *PKERNEL_MEMORY_REQUEST; 