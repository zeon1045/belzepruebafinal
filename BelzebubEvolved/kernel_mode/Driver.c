#include <ntifs.h>
#include "common.h"

// Prototipos de funciones del driver
VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

// Punto de entrada principal del driver
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    UNICODE_STRING devName, symLink;
    RtlInitUnicodeString(&devName, L"\\Device\\BelzebubDriver");
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\BelzebubDriver");

    PDEVICE_OBJECT deviceObject = NULL;
    NTSTATUS status = IoCreateDevice(DriverObject, 0, &devName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Fallo al crear el dispositivo\n"));
        return status;
    }

    status = IoCreateSymbolicLink(&symLink, &devName);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Fallo al crear el enlace simbólico\n"));
        IoDeleteDevice(deviceObject);
        return status;
    }

    // Asignar funciones a eventos del sistema
    DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
    DriverObject->DriverUnload = DriverUnload;

    KdPrint(("Driver Belzebub cargado exitosamente.\n"));
    return STATUS_SUCCESS;
}

// Limpieza al descargar el driver
VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
    UNICODE_STRING symLink;
    RtlInitUnicodeString(&symLink, L"\\DosDevices\\BelzebubDriver");
    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);
    KdPrint(("Driver Belzebub descargado.\n"));
}

// Maneja la apertura y cierre de handles al driver
NTSTATUS CreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

// El cerebro del driver: procesa los comandos de la aplicación user-mode
NTSTATUS DeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;

    if (stack) {
        PKERNEL_MEMORY_REQUEST request = (PKERNEL_MEMORY_REQUEST)Irp->AssociatedIrp.SystemBuffer;
        PEPROCESS targetProcess = NULL;

        // Buscar el proceso objetivo por su PID
        status = PsLookupProcessByProcessId(request->ProcessId, &targetProcess);
        if (!NT_SUCCESS(status)) {
            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            return status;
        }

        switch (stack->Parameters.DeviceIoControl.IoControlCode) {
            case IOCTL_READ_PROCESS_MEMORY: {
                MmCopyVirtualMemory(targetProcess, (PVOID)request->Address, PsGetCurrentProcess(), (PVOID)request->Buffer, request->Size, KernelMode, &request->Size);
                break;
            }
            case IOCTL_WRITE_PROCESS_MEMORY: {
                MmCopyVirtualMemory(PsGetCurrentProcess(), (PVOID)request->Buffer, targetProcess, (PVOID)request->Address, request->Size, KernelMode, &request->Size);
                break;
            }
            default:
                status = STATUS_INVALID_DEVICE_REQUEST;
                break;
        }

        ObDereferenceObject(targetProcess);
    }
    
    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
} 