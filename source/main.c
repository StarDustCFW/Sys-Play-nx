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
u32 list = 0;
u32 isplay = 0;
char filename[32];

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
	if (list <= 0){sprintf(filename, "StarDust/music/fondo.mp3");}
	
	if (music == 1){svcSleepThread(1e+8L);}else{svcSleepThread(1000000000L);}
//		mp3MutInit();

		if (music == 1){
			list++;
			isplay = 1;
			playMp3(filename);
			sprintf(filename, "StarDust/music/fondo%d.mp3", list);
			isplay = 0;
		}
		
//limit of sounds, has fondo(0...1...2).mp3
if (list >= 6){
list = 0;
}
//fix over 
if (isplay >= 2){
isplay = 0;
}
}

}
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    mp3MutInit();
//    pauseInit();
FILE *Play_file = fopen("StarDust/music/pause", "r");
    if (Play_file != NULL)
    {
        music = 0;
        fclose(Play_file);
    }	
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
			FILE *pause_file = fopen("StarDust/music/pause", "w");
			fclose(pause_file);
			audoutExit();
				}else{
			music = 1;
			unlink("/StarDust/music/pause");
			audoutInitialize();
			audoutStartAudioOut();
			}
		}
			    hidScanInput();

	if (music == 1){
		if ((kDown & KEY_ZR|| kDown & KEY_R) && (kHeld & KEY_ZR && kHeld & KEY_R)){
		music = 0;
		audoutExit();
		while (appletMainLoop()){
		svcSleepThread(1e+8L);
		if (isplay == 0){break;}
		}
		audoutInitialize();
		audoutStartAudioOut();
		music = 1;
		}
	}
	
	if (music == 1){
		if ((kDown & KEY_ZL|| kDown & KEY_L) && (kHeld & KEY_ZL && kHeld & KEY_L)){
		music = 0;
		list--;
		list--;
		audoutExit();
		while (appletMainLoop()){
		svcSleepThread(1e+8L);
		if (isplay == 0){break;}
		}
		audoutInitialize();
		audoutStartAudioOut();
		music = 1;
		}
	}
		if ((kDown & KEY_LSTICK || kDown & KEY_RSTICK) && (kHeld & KEY_LSTICK && kHeld & KEY_RSTICK)){break;}

//		if (pid == 0x0100000000001000){break;}
//		if (pid == 0x420000000000000B){break;}	
		
		}
	fsdevUnmountAll();
    fsExit();
    smExit();
    audoutExit();
    timeExit();
    return 0;
}
