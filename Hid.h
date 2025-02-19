#pragma once

#include <ntddk.h>
#include <wdf.h>
#include <hidport.h>

#include "DeviceContext.h"

typedef struct _QUEUE_CONTEXT
{
    WDFQUEUE                Queue;
    PDEVICE_CONTEXT         DeviceContext;
    UCHAR                   OutputReport;

} QUEUE_CONTEXT, * PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, GetQueueContext)

NTSTATUS
QueueCreate(
    _In_  WDFDEVICE         Device,
    _Out_ WDFQUEUE* Queue
);
