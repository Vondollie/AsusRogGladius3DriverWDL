#pragma once
#include <ntddk.h>
#include "DeviceContext.h"

#define LAMP_ARRAY_ATTRIBUTES_REPORT_ID 1
#define LAMP_ATTRIBUTES_REQUEST_REPORT_ID 2
#define LAMP_ATTRIBUTES_RESPONSE_REPORT_ID 3
#define LAMP_MULTI_UPDATE_REPORT_ID 4
#define LAMP_RANGE_UPDATE_REPORT_ID 5
#define LAMP_ARRAY_CONTROL_REPORT_ID 6

#define LAMP_MULTI_UPDATE_LAMP_COUNT 8

enum LampPurposeKind
{
    LampPurposeControl = 1,
    LampPurposeAccent = 2,
    LampPurposeBranding = 4,
    LampPurposeStatus = 8,
    LampPurposeIllumination = 16,
    LampPurposePresentation = 32,
};

enum LampArrayKind
{
    LampArrayKindKeyboard = 1,
    LampArrayKindMouse = 2,
    LampArrayKindGameController = 3,
    LampArrayKindPeripheral = 4,
    LampArrayKindScene = 5,
    LampArrayKindNotification = 6,
    LampArrayKindChassis = 7,
    LampArrayKindWearable = 8,
    LampArrayKindFurniture = 9,
    LampArrayKindArt = 10,
};

#pragma pack(push, 1)
typedef struct
{
    UINT8 red;
    UINT8 green;
    UINT8 blue;
    UINT8 intensity;
} LampColor;


typedef struct
{
    UINT32 x;
    UINT32 y;
    UINT32 z;
} Position;

typedef struct
{
    UINT16 lampCount;

    UINT32 width;
    UINT32 height;
    UINT32 depth;

    UINT32 lampArrayKind;
    UINT32 minUpdateInterval;
} LampArrayAttributesReport;

typedef struct
{
    UINT16 lampId;
} LampAttributesRequestReport;

typedef struct
{
    UINT16 lampId;

    Position lampPosition;

    UINT32 updateLatency;
    UINT32 lampPurposes;

    UINT8 redLevelCount;
    UINT8 greenLevelCount;
    UINT8 blueLevelCount;
    UINT8 intensityLevelCount;

    UINT8 isProgrammable;
    UINT8 inputBinding;
} LampAttributesResponseReport;

typedef struct
{
    UINT8 lampCount;
    UINT8 flags;
    UINT16 lampIds[LAMP_MULTI_UPDATE_LAMP_COUNT];

    LampColor colors[LAMP_MULTI_UPDATE_LAMP_COUNT];
} LampMultiUpdateReport;

typedef struct
{
    UINT8 flags;
    UINT16 lampIdStart;
    UINT16 lampIdEnd;

    LampColor color;
} LampRangeUpdateReport;

typedef struct
{
    UINT8 autonomousMode;
} LampArrayControlReport;
#pragma pack(pop)

NTSTATUS ProcessLampArrayAttributesReport(PHID_XFER_PACKET HidTransferPacket);
NTSTATUS ProcessLampAttributesReport(PHID_XFER_PACKET HidTransferPacket);

NTSTATUS OnLampAttributesRequestReport(PHID_XFER_PACKET HidTransferPacket);
NTSTATUS OnLampArrayControlReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket);

NTSTATUS OnLampMultiUpdateReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket);
NTSTATUS OnLampRangeUpdateReport(PDEVICE_CONTEXT VhfClientContext, PHID_XFER_PACKET HidTransferPacket);