#pragma once
#include <ntddk.h>
#include <wdf.h>
#include <hidpddi.h>
#include <hidclass.h>
#include <hidport.h>

#include "DeviceContext.h"

NTSTATUS HidSetFeature(PDEVICE_CONTEXT DeviceContext, PVOID ReportData, ULONG ReportLength);