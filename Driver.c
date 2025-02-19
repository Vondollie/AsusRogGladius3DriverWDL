#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>

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

    //attributes.EvtCleanupCallback = EvtDeviceContextCleanup;

    // Создание устройства
    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create device: 0x%X\n", status));
        return status;
    }

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

