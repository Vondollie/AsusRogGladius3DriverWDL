#include <ntddk.h>
#include <wdf.h>

#include "Device.h"
#include "DeviceContext.h"

NTSTATUS
HIDINJECTOR_EvtDeviceSelfManagedIoInit(
    WDFDEVICE WdfDevice
)
{
    PDEVICE_CONTEXT	deviceContext;
    NTSTATUS        status;

    PAGED_CODE();

    deviceContext = DeviceGetContext(WdfDevice);

    status = VhfStart(deviceContext->VhfHandle);

    if (!NT_SUCCESS(status))
    {
        // TODO; seach all KDPrint Calls to make sure parameters match format specifiers
        KdPrint(("VhfStart failed %d\n", status));
    }

    KdPrint(("Vhf Started: 0x%X\n", status));

    return status;
}

VOID
HIDINJECTOR_EvtDeviceSelfManagedIoCleanup(
    WDFDEVICE WdfDevice
)
{
    PDEVICE_CONTEXT	deviceContext;

    PAGED_CODE();

    deviceContext = DeviceGetContext(WdfDevice);

    VhfDelete(deviceContext->VhfHandle, TRUE);

    return;
}

// Обработчик добавления устройства
NTSTATUS EvtDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit) {
    UNREFERENCED_PARAMETER(Driver);
    PAGED_CODE();

    WDF_OBJECT_ATTRIBUTES attributes;
    WDFDEVICE device;
    NTSTATUS status;

    WDF_PNPPOWER_EVENT_CALLBACKS wdfPnpPowerCallbacks;

    WdfFdoInitSetFilter(DeviceInit);

    WDF_PNPPOWER_EVENT_CALLBACKS_INIT(&wdfPnpPowerCallbacks);
    wdfPnpPowerCallbacks.EvtDeviceSelfManagedIoInit = HIDINJECTOR_EvtDeviceSelfManagedIoInit;
    wdfPnpPowerCallbacks.EvtDeviceSelfManagedIoCleanup = HIDINJECTOR_EvtDeviceSelfManagedIoCleanup;
    WdfDeviceInitSetPnpPowerEventCallbacks(DeviceInit, &wdfPnpPowerCallbacks);

    // Инициализация атрибутов устройства
    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);

    // Создание устройства
    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create device: 0x%X\n", status));
        return status;
    }

    WDFIOTARGET ioTarget = WdfDeviceGetIoTarget(device);
    
    if (!ioTarget)
    {
        KdPrint(("Failed to get I/O target.\n"));
        return STATUS_INVALID_DEVICE_STATE;
    }

    //WDFIOTARGET ioTarget;
    //WDF_IO_TARGET_OPEN_PARAMS openParams;

    //status = WdfIoTargetCreate(device, WDF_NO_OBJECT_ATTRIBUTES, &ioTarget);
    //if (!NT_SUCCESS(status)) {
    //    KdPrint(("WdfIoTargetCreate failed: 0x%X\n", status));
    //    return status;
    //}

    //PDEVICE_OBJECT pdev = WdfDeviceWdmGetDeviceObject(device);
    //WDF_IO_TARGET_OPEN_PARAMS_INIT_EXISTING_DEVICE(&openParams, pdev);

    //status = WdfIoTargetOpen(
    //    ioTarget,
    //    &openParams
    //);

    //if (!NT_SUCCESS(status)) {
    //    KdPrint(("WdfIoTargetOpen failed: 0x%X\n", status));
    //    return status;
    //}


    PDEVICE_CONTEXT context = DeviceGetContext(device);
    context->ioTarget = ioTarget;

    //WDFQUEUE queue;
    //status = RawQueueCreate(device, &queue);
    //context->Queue = queue;

    // Создание виртуального HID-устройства
    status = CreateVirtualHidDevice(device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to initialize virtual HID device: 0x%X\n", status));
        return status;
    }

    return STATUS_SUCCESS;
}


// Точка входа драйвера
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    // Инициализация конфигурации KMDF
    WDF_DRIVER_CONFIG_INIT(&config, EvtDeviceAdd);

    // Создание драйвера
    status = WdfDriverCreate(DriverObject, RegistryPath, WDF_NO_OBJECT_ATTRIBUTES, &config, WDF_NO_HANDLE);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create driver: 0x%X\n", status));
        return status;
    }

    KdPrint(("Driver loaded successfully.\n"));
    return STATUS_SUCCESS;
}

