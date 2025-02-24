#include "Hid.h"

NTSTATUS SendHidOutputReportToDevice(WDFIOTARGET IoTarget, PVOID ReportData, ULONG ReportLength)
{
    NTSTATUS status;
    WDF_MEMORY_DESCRIPTOR memoryDescriptor;
    UCHAR* buffer;

    // ��������� ������ ��� ������
    buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, ReportLength, 'HidR');
    if (!buffer)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(buffer, ReportData, ReportLength);

    // ������������� ����������� ������
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&memoryDescriptor, buffer, ReportLength);

    // �������� IOCTL �������
    status = WdfIoTargetSendIoctlSynchronously(
        IoTarget,
        NULL, // WDFREQUEST (�� ��������� ��� ����������� ������)
        IOCTL_HID_SET_OUTPUT_REPORT,
        &memoryDescriptor,
        NULL, // Optional output buffer
        NULL, // Optional output buffer length
        NULL  // Optional timeout
    );

    // ������������ ������
    ExFreePoolWithTag(buffer, 'HidR');

    return status;
}