#include <string.h>
#include <stdio.h>
#include <dirent.h>

#include <errno.h>
#include <malloc.h>
#include <stdarg.h>
#include <unistd.h>
#include <malloc.h>
#include <switch.h>

#include <stdlib.h>

// only for mkdir, used when creating the "logs" directory
#include <sys/stat.h>

#include <switch.h>

#include "util.h"

#include "mp3.h"

#define TITLE_ID 0x420000000000000B
#define HEAP_SIZE 0x000240000

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
			audoutInitialize();
			audoutStartAudioOut();
u32 cutl = 0;		
	
    while (appletMainLoop())
    {
	if (list <= 0){sprintf(filename, "StarDust/music/fondo.mp3");}
	
	svcSleepThread(1000000000L);
//		mp3MutInit();
		if(cutl == 0){
		cutl++;
		}
		if (music == 1){
			list++;
			isplay = 1;
			if (file_exist(filename))
			{
			playMp3("StarDust/music/inputok.mp3");
			playMp3(filename);
			}
			sprintf(filename, "StarDust/music/fondo%d.mp3", list);
			isplay = 0;
		}
		
//limit of sounds, has fondo(0...1...2).mp3
if (list >= 7){
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
	hidInitialize();
    mp3MutInit();
//    pauseInit();
    if (file_exist("StarDust/music/stop"))
    {
        music = 0;
    }
	unlink("/StarDust/music/pause");//just in case
	Thread pauseThread;
    Result rc = threadCreate(&pauseThread, wakey, NULL, 0x4000, 49, 3);
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = threadStart(&pauseThread);
    if (R_FAILED(rc))
        fatalLater(rc);
    while (appletMainLoop())
    {
	    svcSleepThread(1000000000L);
	    hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
			//Stop
			if (music == 1)
			{
				if ((kDown & KEY_R || kDown & KEY_L || kDown & KEY_B) && (kHeld & KEY_R && kHeld & KEY_L && kHeld & KEY_B))
				{
				music = 0;
				led_on(1);
				list--;
				audoutStopAudioOut();
				create_flag("StarDust/music/stop");
				audoutExit();
				}
			}
			
		//Pause
        if ((kDown & KEY_R || kDown & KEY_L || kDown & KEY_X) && (kHeld & KEY_R && kHeld & KEY_L && kHeld & KEY_X)){
			if (music == 1)
			{music = 0;
			led_on(1);
			audoutStopAudioOut();
			create_flag("StarDust/music/pause");
			audoutExit();
				}else{
			music = 1;
			led_on(0);
			audoutInitialize();
			unlink("/StarDust/music/pause");
			unlink("/StarDust/music/stop");
			audoutStartAudioOut();
			}
		}

	//next
	if (music == 1){
		if ((kDown & KEY_ZR|| kDown & KEY_R) && (kHeld & KEY_ZR && kHeld & KEY_R)){
		music = 0;
		create_flag("StarDust/music/stop");
		audoutStopAudioOut();
		while (appletMainLoop()){
		svcSleepThread(1000000000L);
		if (isplay == 0){break;}
		}
		unlink("/StarDust/music/stop");
		audoutStartAudioOut();
		music = 1;
		}
	}

	//prev
	if (music == 1){
		if ((kDown & KEY_ZL|| kDown & KEY_L) && (kHeld & KEY_ZL && kHeld & KEY_L)){
		music = 0;
		list--;
		list--;
		create_flag("StarDust/music/stop");
		audoutStopAudioOut();
		while (appletMainLoop()){
		svcSleepThread(1000000000L);
		if (isplay == 0){break;}
		}
		unlink("/StarDust/music/stop");
		audoutStartAudioOut();
		music = 1;
		}
	}

	//kill service
	if ((kDown & KEY_LSTICK || kDown & KEY_RSTICK) && (kHeld & KEY_LSTICK && kHeld & KEY_RSTICK)){
	break;}


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
