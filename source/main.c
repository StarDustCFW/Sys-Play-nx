#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <unistd.h>
#include "console.h"


// only for mkdir, used when creating the "logs" directory
#include <sys/stat.h>

#include <switch.h>

#include "util.h"

#include "mp3.h"

#define TITLE_ID 0x420000000000000B
#define HEAP_SIZE 0x000380000

// we aren't an applet
u32 __nx_applet_type = AppletType_None;

// setup a fake heap
char fake_heap[HEAP_SIZE];

//main Switch
u32 music = 1;


// we override libnx internals to do a minimal init
void __libnx_initheap(void)
{
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    // setup newlib fake heap
    fake_heap_start = fake_heap;
    fake_heap_end = fake_heap + HEAP_SIZE;
}

void __appInit(void)
{
    Result rc;
    svcSleepThread(10000000000L);
    rc = smInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = fsdevMountSdmc();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = timeInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = hidInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
}

void wakey()
{
    while (appletMainLoop())
    {
		svcSleepThread(1000000000L);
//		mp3MutInit();
		if (music == 1)
		playMp3("modules/music/fondo.mp3");
		if (music == 1)
		playMp3("modules/music/fondo2.mp3");
		if (music == 1)
		playMp3("modules/music/fondo3.mp3");
		if (music == 1)
		playMp3("modules/music/fondo4.mp3");
		if (music == 1)
		playMp3("modules/music/fondo5.mp3");
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mp3MutInit();
//    pauseInit();
	Thread pauseThread;
    Result rc = threadCreate(&pauseThread, wakey, NULL, 0x4000, 49, 3);
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = threadStart(&pauseThread);
    if (R_FAILED(rc))
        fatalLater(rc);
    while (appletMainLoop())
    {
	    svcSleepThread(1e+8L);

	    hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
        if ((kDown & KEY_R || kDown & KEY_L || kDown & KEY_X) && (kHeld & KEY_R && kHeld & KEY_L && kHeld & KEY_X)){
			if (music == 1)
			{music = 0;
			audoutExit();
				}else{
			music = 1;
			audoutInitialize();
			audoutStartAudioOut();
			}
		}
			    hidScanInput();

		if ((kDown & KEY_LSTICK || kDown & KEY_RSTICK) && (kHeld & KEY_LSTICK && kHeld & KEY_RSTICK)){break;}

	}
	fsdevUnmountAll();
    fsExit();
    smExit();
    audoutExit();
    timeExit();
    return 0;
}
