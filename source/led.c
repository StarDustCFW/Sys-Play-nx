#include <string.h>
#include <switch.h>

#include "util.h"

void flash_led_connect()
{
    HidsysNotificationLedPattern pattern;
				memset(&pattern, 0, sizeof(pattern));
				// Setup Heartbeat effect pattern data.
				pattern.baseMiniCycleDuration = 0x1;             // 12.5ms.
				pattern.totalMiniCycles = 0xF;                   // 16 mini cycles.
				pattern.totalFullCycles = 0x1;                   // Repeat forever.
				pattern.startIntensity = 0x0;                    // 0%.

				// First beat.
				pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
				pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
				pattern.miniCycles[1].ledIntensity = 0x0;        // 0%.
				pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.

    u64 uniquePadIds[5] = {0};

    size_t total_entries = 0;

    Result rc = hidsysGetUniquePadIds(uniquePadIds, 5, &total_entries);
    if (R_FAILED(rc) && rc != MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer))
        fatalLater(rc);

    for (int i = 0; i < total_entries; i++)
        hidsysSetNotificationLedPattern(&pattern, uniquePadIds[i]);
}

void flash_led_disconnect()
{
    HidsysNotificationLedPattern pattern;
    memset(&pattern, 0, sizeof(pattern));

    u64 uniquePadIds[2];
    memset(uniquePadIds, 0, sizeof(uniquePadIds));

    size_t total_entries = 0;

    Result rc = hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, uniquePadIds, 2, &total_entries);
    if (R_FAILED(rc) && rc != MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer))
        fatalLater(rc);

    for (int i = 0; i < total_entries; i++)
        hidsysSetNotificationLedPattern(&pattern, uniquePadIds[i]);
}
