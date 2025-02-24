#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <hidclass.h>

NTSTATUS SendHidOutputReportToDevice(WDFIOTARGET IoTarget, PVOID ReportData, ULONG ReportLength);
