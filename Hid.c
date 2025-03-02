#include "Hid.h"

NTSTATUS HidSetFeature(PDEVICE_CONTEXT DeviceContext, PVOID ReportData, ULONG ReportLength) {
    NTSTATUS status;
    WDF_MEMORY_DESCRIPTOR inputMemoryDescriptor;

    PAGED_CODE();

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputMemoryDescriptor,
        ReportData,
        ReportLength);

    WDF_REQUEST_SEND_OPTIONS sendOptions;

    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    // Отправка IOCTL запроса
    status = WdfIoTargetSendIoctlSynchronously(
        DeviceContext->IoTarget,
        NULL, // WDFREQUEST (не требуется для синхронного вызова)
        IOCTL_HID_SET_OUTPUT_REPORT,
        &inputMemoryDescriptor,
        NULL, // Optional output buffer
        &sendOptions, // REquestOPty
        NULL  // BytesReturned
    );

    return status;
}