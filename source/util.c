#include <switch.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <switch.h>

// only for mkdir, used when creating the "logs" directory
#include <sys/stat.h>
void fatalLater(Result err)
{
    Handle srv;

    while (R_FAILED(smGetServiceOriginal(&srv, smEncodeName("fatal:u"))))
    {
        // wait one sec and retry
        svcSleepThread(1000000000L);
    }

    // fatal is here time, fatal like a boss
    IpcCommand c;
    ipcInitialize(&c);
    ipcSendPid(&c);
    struct
    {
        u64 magic;
        u64 cmd_id;
        u64 result;
        u64 unknown;
    } * raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
    raw->result = err;
    raw->unknown = 0;

    ipcDispatch(srv);
    svcCloseHandle(srv);
}

bool file_exist(char* File)
{
FILE *Play_file = fopen(File, "r");
    if (Play_file != NULL)
	{
	fclose(Play_file);
	return true;
    }
	return false;
}

void create_flag(char* File)
{
FILE *Play_file = fopen(File, "w");
fclose(Play_file);
}

bool led_on(u64 inter)
{
			Result rc=0;
				size_t i;
				size_t total_entries;
				u64 UniquePadIds[2];
				HidsysNotificationLedPattern pattern;
				rc = hidsysInitialize();
				if (R_FAILED(rc)) {
					printf("hidsysInitialize(): 0x%x\n", rc);
				}
	switch(inter)
	{
		case 1:

				memset(&pattern, 0, sizeof(pattern));
				// Setup Breathing effect pattern data.
				pattern.baseMiniCycleDuration = 0x8;             // 100ms.
				pattern.totalMiniCycles = 0x2;                   // 3 mini cycles. Last one 12.5ms.
				pattern.totalFullCycles = 0x0;                   // Repeat forever.
				pattern.startIntensity = 0x2;                    // 13%.

				pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
				pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Transition time 1.5s.
				pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
				pattern.miniCycles[1].ledIntensity = 0x2;        // 13%.
				pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Transition time 1.5s.
				pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.
				
				rc = hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, UniquePadIds, 2, &total_entries);

				if (R_SUCCEEDED(rc)) {
					for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
						rc = hidsysSetNotificationLedPattern(&pattern, UniquePadIds[i]);
					}
				}
			hidsysExit();
		break;
	case 2:
				memset(&pattern, 0, sizeof(pattern));
				// Setup Heartbeat effect pattern data.
				pattern.baseMiniCycleDuration = 0x1;             // 12.5ms.
				pattern.totalMiniCycles = 0xF;                   // 16 mini cycles.
				pattern.totalFullCycles = 0x0;                   // Repeat forever.
				pattern.startIntensity = 0x0;                    // 0%.

				// First beat.
				pattern.miniCycles[0].ledIntensity = 0xF;        // 100%.
				pattern.miniCycles[0].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[0].finalStepDuration = 0x0;   // Forced 12.5ms.
				pattern.miniCycles[1].ledIntensity = 0x0;        // 0%.
				pattern.miniCycles[1].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
				pattern.miniCycles[1].finalStepDuration = 0x0;   // Forced 12.5ms.

				// Second beat.
				pattern.miniCycles[2].ledIntensity = 0xF;
				pattern.miniCycles[2].transitionSteps = 0xF;
				pattern.miniCycles[2].finalStepDuration = 0x0;
				pattern.miniCycles[3].ledIntensity = 0x0;
				pattern.miniCycles[3].transitionSteps = 0xF;
				pattern.miniCycles[3].finalStepDuration = 0x0;

				// Led off wait time.
				for(i=2; i<4; i++) {
					pattern.miniCycles[i].ledIntensity = 0x0;        // 0%.
					pattern.miniCycles[i].transitionSteps = 0xF;     // 15 steps. Total 187.5ms.
					pattern.miniCycles[i].finalStepDuration = 0xF;   // 187.5ms.
				}
			
					
				rc = hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, UniquePadIds, 2, &total_entries);
				if (R_SUCCEEDED(rc)) {
					for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
						rc = hidsysSetNotificationLedPattern(&pattern, UniquePadIds[i]);
					}
				}
			hidsysExit();
	break;
	case 0:
	default:
				memset(&pattern, 0, sizeof(pattern));
				total_entries = 0;
				memset(UniquePadIds, 0, sizeof(UniquePadIds));
				rc = hidsysGetUniquePadsFromNpad(hidGetHandheldMode() ? CONTROLLER_HANDHELD : CONTROLLER_PLAYER_1, UniquePadIds, 2, &total_entries);

				if (R_SUCCEEDED(rc)) {
					for(i=0; i<total_entries; i++) { // System will skip sending the subcommand to controllers where this isn't available.
						rc = hidsysSetNotificationLedPattern(&pattern, UniquePadIds[i]);
					}
				}
			hidsysExit();				
	}

return 0;
}
