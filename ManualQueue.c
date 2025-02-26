#include "ManualQueue.h"

VOID EvtTimerFunc(WDFTIMER Timer) {
    NTSTATUS                status;
    WDFQUEUE                queue;
    WDFREQUEST              request;

    queue = (WDFQUEUE)WdfTimerGetParentObject(Timer);
    status = WdfIoQueueRetrieveNextRequest(queue, &request);

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_NO_MORE_ENTRIES) {
            KdPrint(("������� �����. ��� ������� ����������.\n"));
            return;
        }
        else {
            KdPrint(("������ ��������� ���������� �������: 0x%X\n", status));
            return;
        }
    }

    KdPrint(("������� ������ � �������.\n"));

    WdfRequestComplete(request, status);
}


NTSTATUS CreateManualQueue(WDFDEVICE device, WDFQUEUE* pQueue) {
    WDF_IO_QUEUE_CONFIG queueConfig;
    WDFTIMER timer;
    WDF_TIMER_CONFIG timerConfig;
    WDF_OBJECT_ATTRIBUTES timerAttributes;
    ULONG timerPeriodInMillieconds = 10;
    NTSTATUS status;

    // ������������� ������������ �������
    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

    // �������� �������
    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, pQueue);
    if (!NT_SUCCESS(status)) {
        KdPrint(("������ ��� �������� �������: 0x%X\n", status));
        return status;
    }

    WDF_TIMER_CONFIG_INIT_PERIODIC(
        &timerConfig,
        EvtTimerFunc,
        timerPeriodInMillieconds);
    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = *pQueue;
    status = WdfTimerCreate(&timerConfig,
        &timerAttributes,
        &timer);

    if (!NT_SUCCESS(status)) {
        KdPrint(("WdfTimerCreate failed 0x%x\n", status));
        return status;
    }

    WdfTimerStart(timer, WDF_REL_TIMEOUT_IN_MS(timerPeriodInMillieconds));

    return STATUS_SUCCESS;
}