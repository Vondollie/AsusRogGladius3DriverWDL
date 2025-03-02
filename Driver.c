#include <ntddk.h>
#include <wdf.h>

#include "Device.h"
#include "DeviceContext.h"

static NTSTATUS
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

static VOID
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


static NTSTATUS CreatePdoIoTarget(WDFDEVICE device, WDFIOTARGET* ioTarget) {
    NTSTATUS status;
    WDFMEMORY memory;
    size_t bufferLength;
    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_IO_TARGET_OPEN_PARAMS openParams;

    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);

    attributes.ParentObject = device;

    status = WdfDeviceAllocAndQueryProperty(device,
        DevicePropertyPhysicalDeviceObjectName,
        NonPagedPoolNx,
        &attributes,
        &memory);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    UNICODE_STRING pdoName;
    pdoName.Buffer = WdfMemoryGetBuffer(memory, &bufferLength);
    if (pdoName.Buffer == NULL) {
        return STATUS_UNSUCCESSFUL;
    }
    pdoName.MaximumLength = (USHORT)bufferLength;
    pdoName.Length = (USHORT)bufferLength - sizeof(UNICODE_NULL);

    status = WdfIoTargetCreate(device,
        WDF_NO_OBJECT_ATTRIBUTES,
        ioTarget);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
        &openParams,
        &pdoName,
        FILE_WRITE_ACCESS);

    openParams.ShareAccess = FILE_SHARE_WRITE | FILE_SHARE_READ;

    status = WdfIoTargetOpen(*ioTarget, &openParams);
    
    return status;
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

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes, DEVICE_CONTEXT);

    // Create device
    status = WdfDeviceCreate(&DeviceInit, &attributes, &device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create device: 0x%X\n", status));
        return status;
    }

    PDEVICE_CONTEXT context = DeviceGetContext(device);

    // Get PDO io target
    status = CreatePdoIoTarget(device, &context->IoTarget);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to create PDO io target: 0x%X\n", status));
        return status;
    }

    // Create virtual HID device
    status = CreateVirtualHidDevice(device);
    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to initialize virtual HID device: 0x%X\n", status));
        return status;
    }

    return STATUS_SUCCESS;
}

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

