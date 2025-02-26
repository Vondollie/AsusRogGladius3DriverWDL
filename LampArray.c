#include "Hid.h"
#include "LampArray.h"
#include "DeviceContext.h"


#define LAMP_UPDATE_LATENCY 10000
#define LAMP_COUNT 3
#define BOUND_WIDTH 68000
#define BOUND_HEIGHT 123000
#define BOUND_DEPTH 44000
#define UPDATE_INTERVAL 30000

static LampAttributesResponseReport lampAttributes[] =
{
    // All positions in millimeters from upper-left corner of device.
    // All times in milliseconds.
    // Id  X     Y     Z     Latency                        Purposes           RED   GRN   BLUE  GAIN  PROGAMMBLE?           KEY
    {0x00, 30000, 25000, 30000, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x01, 0, 70000, 15000, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x02, 34000, 100000, 30000, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    /*{0x03, 4000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x04, 5000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x05, 6000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x06, 7000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x07, 8000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},*/
};

NTSTATUS ProcessLampArrayAttributesReport(PHID_XFER_PACKET HidTransferPacket) {

    const LampArrayAttributesReport lampArrayAttributesReport =
    {
        LAMP_COUNT,
        BOUND_WIDTH,
        BOUND_HEIGHT,
        BOUND_DEPTH,
        LampArrayKindMouse,
        UPDATE_INTERVAL,
    };

    RtlFillMemory(HidTransferPacket->reportBuffer, HidTransferPacket->reportBufferLen, 0);
    HidTransferPacket->reportBuffer[0] = HidTransferPacket->reportId;
    RtlCopyMemory(&HidTransferPacket->reportBuffer[1], &lampArrayAttributesReport, sizeof(LampArrayAttributesReport));

    return STATUS_SUCCESS;
}

UINT16 lastLampIdRequested = 0;

NTSTATUS ProcessLampAttributesReport(PHID_XFER_PACKET HidTransferPacket) {

    RtlFillMemory(HidTransferPacket->reportBuffer, HidTransferPacket->reportBufferLen, 0);
    HidTransferPacket->reportBuffer[0] = HidTransferPacket->reportId;
    RtlCopyMemory(&HidTransferPacket->reportBuffer[1], &lampAttributes[lastLampIdRequested], sizeof(LampAttributesResponseReport));
    lastLampIdRequested++;
    if (lastLampIdRequested >= LAMP_COUNT)
    {
        // Reset to zero
        lastLampIdRequested = 0;
    }

    return STATUS_SUCCESS;
}

NTSTATUS OnLampAttributesRequestReport(PHID_XFER_PACKET HidTransferPacket) {
    LampAttributesRequestReport* report = (LampAttributesRequestReport*)&HidTransferPacket->reportBuffer[1];

    if (report->lampId < LAMP_COUNT)
    {
        lastLampIdRequested = report->lampId;
    }
    else
    {
        lastLampIdRequested = 0;
    }

    return STATUS_SUCCESS;
}

NTSTATUS OnLampArrayControlReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket) {
    NTSTATUS status = STATUS_SUCCESS;

    LampArrayControlReport* report = (LampArrayControlReport*)&HidTransferPacket->reportBuffer[1];

    if (report->autonomousMode == TRUE) {
        for (UINT8 i = 0; i < LAMP_COUNT; i++)
        {
            UCHAR buffer[65];
            RtlFillMemory(buffer, 65, 0);
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x51;
            buffer[0x02] = 0x28;
            buffer[0x03] = i;
            buffer[0x04] = 0x00;
            buffer[0x05] = 0x00;
            buffer[0x06] = 0x64;
            buffer[0x07] = 0x00;
            buffer[0x08] = 0x00;
            buffer[0x09] = 0x00;

            status = HidSetFeature(VhfClientContext, buffer, 65);

            LARGE_INTEGER interval;
            interval.QuadPart = -(10 * 1000 * 10); // Преобразование миллисекунд в 100-наносекундные интервалы

            KeDelayExecutionThread(KernelMode, FALSE, &interval);
        }
    }

    return status;
}

NTSTATUS OnLampMultiUpdateReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket) {
    NTSTATUS status = STATUS_SUCCESS;
    LampMultiUpdateReport* report = (LampMultiUpdateReport*)&HidTransferPacket->reportBuffer[1];

    for (UINT8 i = 0; i < report->lampCount; i++)
    {
        if (report->lampIds[i] < LAMP_COUNT)
        {
            UCHAR buffer[65];
            RtlFillMemory(buffer, 65, 0);
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x51;
            buffer[0x02] = 0x28;
            buffer[0x03] = i;
            buffer[0x04] = 0x00;
            buffer[0x05] = 0x00;
            buffer[0x06] = 0x64;
            buffer[0x07] = report->colors->red;
            buffer[0x08] = report->colors->green;
            buffer[0x09] = report->colors->blue;

            status = HidSetFeature(VhfClientContext, buffer, 65);

            LARGE_INTEGER interval;
            interval.QuadPart = -(10 * 1000 * 10); // Преобразование миллисекунд в 100-наносекундные интервалы

            KeDelayExecutionThread(KernelMode, FALSE, &interval);
        }
    }

    return status;
}

NTSTATUS OnLampRangeUpdateReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket) {
    NTSTATUS status = STATUS_SUCCESS;

    LampRangeUpdateReport* report = (LampRangeUpdateReport*)&HidTransferPacket->reportBuffer[1];

    if (report->lampIdStart >= 0 && report->lampIdStart < LAMP_COUNT &&
        report->lampIdEnd >= 0 && report->lampIdEnd < LAMP_COUNT &&
        report->lampIdStart <= report->lampIdEnd)
    {
        for (UINT16 i = report->lampIdStart; i <= report->lampIdEnd; i++)
        {
            UCHAR buffer[65];
            RtlFillMemory(buffer, 65, 0);
            buffer[0x00] = 0x00;
            buffer[0x01] = 0x51;
            buffer[0x02] = 0x28;
            buffer[0x03] = (UINT8)i;
            buffer[0x04] = 0x00;
            buffer[0x05] = 0x00;
            buffer[0x06] = 0x64;
            buffer[0x07] = report->color.red;
            buffer[0x08] = report->color.green;
            buffer[0x09] = report->color.blue;


            status = HidSetFeature(VhfClientContext, buffer, 65);

            LARGE_INTEGER interval;
            interval.QuadPart = -(10 * 1000 * 10); // Преобразование миллисекунд в 100-наносекундные интервалы

            KeDelayExecutionThread(KernelMode, FALSE, &interval);
        }
    }

    return status;
}