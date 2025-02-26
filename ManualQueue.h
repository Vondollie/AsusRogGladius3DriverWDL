#pragma once
#include <ntddk.h>
#include <wdf.h>

VOID EvtTimerFunc(WDFTIMER Timer);
NTSTATUS CreateManualQueue(WDFDEVICE device, WDFQUEUE* pQueue);