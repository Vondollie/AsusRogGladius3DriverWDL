#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <vhf.h>
typedef UCHAR HID_REPORT_DESCRIPTOR, * PHID_REPORT_DESCRIPTOR;

typedef struct _DEVICE_CONTEXT
{
    WDFDEVICE       Device;
    VHFHANDLE       VhfHandle;
    WDFIOTARGET     ioTarget;
    WDFQUEUE        Queue; 

} DEVICE_CONTEXT, * PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, DeviceGetContext)