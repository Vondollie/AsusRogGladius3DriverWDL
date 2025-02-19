#include "LampArray.h"

#define LAMP_UPDATE_LATENCY 4000
#define LAMP_COUNT 8
#define BOUND_WIDTH 40000
#define BOUND_HEIGHT 70000
#define BOUND_DEPTH 55000
#define UPDATE_INTERVAL 33000

static LampAttributesResponseReport lampAttributes[] =
{
    // All positions in millimeters from upper-left corner of device.
    // All times in milliseconds.
    // Id  X     Y     Z     Latency                        Purposes           RED   GRN   BLUE  GAIN  PROGAMMBLE?           KEY
    {0x00, 1000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x01, 2000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x02, 3000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x03, 4000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x04, 5000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x05, 6000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x06, 7000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
    {0x07, 8000, 8000, 0, LAMP_UPDATE_LATENCY, LampPurposeAccent, 0xFF, 0xFF, 0xFF, 0x01, 1, 0x00},
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

NTSTATUS OnLampArrayControlReport(PHID_XFER_PACKET HidTransferPacket) {
    UNREFERENCED_PARAMETER(HidTransferPacket);
    return STATUS_SUCCESS;
}

NTSTATUS OnLampMultiUpdateReport(PHID_XFER_PACKET HidTransferPacket) {
    UNREFERENCED_PARAMETER(HidTransferPacket);
    return STATUS_SUCCESS;
}

NTSTATUS OnLampRangeUpdateReport(PHID_XFER_PACKET HidTransferPacket) {
    UNREFERENCED_PARAMETER(HidTransferPacket);
    return STATUS_SUCCESS;
}