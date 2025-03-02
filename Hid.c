#include "Hid.h"

NTSTATUS HidSetFeature(PDEVICE_CONTEXT DeviceContext, PVOID ReportData, ULONG ReportLength) {
    NTSTATUS                    status;
    WDFIOTARGET                 hidTarget;
    WDF_IO_TARGET_OPEN_PARAMS   openParams;
    WDF_MEMORY_DESCRIPTOR  inputMemoryDescriptor;

    PAGED_CODE();

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&inputMemoryDescriptor,
        ReportData,
        ReportLength);

    hidTarget = NULL;

    status = WdfIoTargetCreate(WdfObjectContextGetObject(DeviceContext),
        WDF_NO_OBJECT_ATTRIBUTES,
        &hidTarget);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
        &openParams,
        &DeviceContext->PdoName,
        FILE_WRITE_ACCESS);

    openParams.ShareAccess = FILE_SHARE_WRITE | FILE_SHARE_READ;

    status = WdfIoTargetOpen(hidTarget, &openParams);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    WDF_REQUEST_SEND_OPTIONS sendOptions;

    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_SEND_AND_FORGET);

    // Отправка IOCTL запроса
    status = WdfIoTargetSendIoctlSynchronously(
        hidTarget,
        NULL, // WDFREQUEST (не требуется для синхронного вызова)
        IOCTL_HID_SET_OUTPUT_REPORT,
        &inputMemoryDescriptor,
        NULL, // Optional output buffer
        &sendOptions, // REquestOPty
        NULL  // BytesReturned
    );

    return status;
}