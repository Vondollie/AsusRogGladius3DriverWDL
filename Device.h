#pragma once
#include <ntddk.h>
#include <wdf.h>

#include "DeviceContext.h"


NTSTATUS CreateVirtualHidDevice(_In_ WDFDEVICE DeviceObject);
EVT_WDF_DEVICE_CONTEXT_CLEANUP EvtDeviceContextCleanup;

EVT_VHF_ASYNC_OPERATION            EvtVhfAsyncOperationGetFeature;
EVT_VHF_ASYNC_OPERATION            EvtVhfAsyncOperationSetFeature;