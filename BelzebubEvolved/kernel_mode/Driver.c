#include <ntifs.h>
#include "common.h" // Incluimos el header compartido.

// Declaraciones de las funciones que el sistema operativo llamará.
VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject);
NTSTATUS CreateCloseDispatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DeviceControlDispatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

// El punto de entrada de nuestro driver. Se ejecuta cuando se carga.
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    KdPrint(("BelzebubDriver: Cargando driver...\n"));

    UNICODE_STRING devName, symLink;
    RtlInitUnicodeString(&devName, L"\\Device\\Belzebub");
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\Belzebub");

    PDEVICE_OBJECT deviceObject = NULL;
    NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) {
        KdPrint(("BelzebubDriver: Fallo al crear el dispositivo (0x%08X)\n", status));
        return status;
    }

    // Asignamos nuestras funciones a los eventos del sistema.
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCloseDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateCloseDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControlDispatch;
    DriverObject->DriverUnload = DriverUnload;

    // Creamos un "acceso directo" para que nuestra app de user-mode pueda encontrar el driver.
    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        KdPrint(("BelzebubDriver: Fallo al crear el enlace simbólico (0x%08X)\n", status));
        IoDeleteDevice(deviceObject);
        return status;
    }

    KdPrint(("BelzebubDriver: Driver cargado exitosamente.\n"));
    return STATUS_SUCCESS;
}

// Se llama cuando se descarga nuestro driver.
VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symLink;
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\Belzebub");
    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    KdPrint(("BelzebubDriver: Driver descargado.\n"));
}

// Maneja cuando la aplicación se conecta o desconecta del driver.
NTSTATUS CreateCloseDispatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// El cerebro del driver: aquí se procesan los comandos de nuestra app.
NTSTATUS DeviceControlDispatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    
    // Obtenemos los datos que nos envió la app.
    PKERNEL_MEMORY_REQUEST request = (PKERNEL_MEMORY_REQUEST)Irp->AssociatedIrp.SystemBuffer;

    if (stack) {
        PEPROCESS targetProcess = NULL;

        // Buscamos el proceso objetivo usando su PID.
        status = PsLookupProcessByProcessId((HANDLE)request->ProcessId, &targetProcess);
        if (NT_SUCCESS(status)) {
            SIZE_T bytesCopied = 0;
            switch (stack->Parameters.DeviceIoControl.IoControlCode) {
                case IOCTL_READ_PROCESS_MEMORY:
                    // Usamos una función segura del kernel para copiar memoria.
                    status = MmCopyVirtualMemory(
                        targetProcess,
                        (PVOID)request->Address,
                        PsGetCurrentProcess(),
                        (PVOID)request->BufferAddress,
                        request->Size,
                        KernelMode,
                        &bytesCopied
                    );
                    break;
                case IOCTL_WRITE_PROCESS_MEMORY:
                    status = MmCopyVirtualMemory(
                        PsGetCurrentProcess(),
                        (PVOID)request->BufferAddress,
                        targetProcess,
                        (PVOID)request->Address,
                        request->Size,
                        KernelMode,
                        &bytesCopied
                    );
                    break;
                default:
                    status = STATUS_INVALID_DEVICE_REQUEST;
                    break;
            }
            // Liberamos la referencia al proceso.
            ObDereferenceObject(targetProcess);
        }
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0; // Opcional: podrías devolver aquí la cantidad de bytes leídos/escritos.
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}