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
#include "led.h"

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
u32 musiclimit = 0;
u32 prevmusic = 0;
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
    rc = hidsysInitialize();
    if (R_FAILED(rc))
        fatalLater(rc);
    rc = setsysInitialize();
    if (R_SUCCEEDED(rc))
    {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }
}

void wakey()
{	//Audio Thread
    while (appletMainLoop())
    {
	svcSleepThread(1000000000L);
		if (music == 1)
		{
			isplay = 1;
			sprintf(filename, "StarDust/music/%d.mp3", list);
			if (file_exist(filename))
			{
				flash_led_connect();
				playMp3(filename);
			}
			if(prevmusic == 0)
			list++;
			else
				if(list <= 0)
					list = musiclimit;
					else
						list--;
			isplay = 0;
		}
		
		//limit of sounds by list
		if (list > musiclimit)
		{
			list = 0;
		}
	}
}

void Scan_folder(){
    //List Audio and rename to use
	musiclimit = 0;
	DIR *dir;
    struct dirent *ent;
    dir = opendir("/StarDust/music");
    while ((ent = readdir(dir)))
    {
        printf(ent->d_name);
        char filename[272];
        char destname[272];
		musiclimit++;
		flash_led_connect();
		if(strlen(ent->d_name) > 6)
		{
			snprintf(filename, 272, "/StarDust/music/%s", ent->d_name);
				for(int i = 0; i < 200; i ++)
				{
					//rename the mp3 for use 
					snprintf(destname, 272, "/StarDust/music/%d.mp3", i);
					if (!file_exist(destname))
					{
						rename(filename, destname); 
						break;
					}
				}
		}
    }

	musiclimit--;//ajust the use of 0.mp3
	if(file_exist("StarDust/music/stop"))
	musiclimit--;
	if(file_exist("StarDust/music/pause"))
	musiclimit--;
			if (file_exist("StarDust/x.log"))
			{
			FILE *f;
			f = fopen("StarDust/x.log", "w+");
			fprintf(f,"---------------..\n");
			fprintf(f, "music limit is %d ..\n",musiclimit);
			fclose(f);
			}
    closedir(dir);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
	hidInitialize();
    mp3MutInit();
//    pauseInit();

unlink("/StarDust/music/pause");//just in case
	audoutInitialize();
	audoutStartAudioOut();
	Scan_folder();

    if (file_exist("StarDust/music/stop"))
    {
        music = 0;
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
	    svcSleepThread(1000000000L);
	    hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
        u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		
			//Stop
			if (music == 1)
			{
				if ((kDown & KEY_R || kDown & KEY_L || kDown & KEY_B) && (kHeld & KEY_R && kHeld & KEY_L && kHeld & KEY_B))
				{
				flash_led_connect();
				music = 0;
				list--;
				audoutStopAudioOut();
				create_flag("StarDust/music/stop");
				audoutExit();
				}
			}
			
		//Pause unestable
        if ((kDown & KEY_R || kDown & KEY_L || kDown & KEY_X) && (kHeld & KEY_R && kHeld & KEY_L && kHeld & KEY_X)){
			flash_led_connect();
			if (music == 1)
			{music = 0;
			audoutStopAudioOut();
			create_flag("StarDust/music/pause");
			audoutExit();
				}else{
			music = 1;
			audoutInitialize();
			unlink("/StarDust/music/pause");
			unlink("/StarDust/music/stop");
			audoutStartAudioOut();
			}
		}

	//next
	if (music == 1){
		if ((kDown & KEY_ZR|| kDown & KEY_R) && (kHeld & KEY_ZR && kHeld & KEY_R)){
		flash_led_connect();
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
		flash_led_connect();
		music = 0;
		prevmusic = 1;
		create_flag("StarDust/music/stop");
		audoutStopAudioOut();
		while (appletMainLoop()){
		svcSleepThread(1000000000L);
		if (isplay == 0){break;}
		}
		unlink("/StarDust/music/stop");
		audoutStartAudioOut();
		prevmusic = 0;
		music = 1;
		}
	}

	if (music == 0){
		if ((kDown & KEY_ZL || kDown & KEY_ZR || kDown & KEY_Y) && (kHeld & KEY_ZL && kHeld & KEY_ZR && kHeld & KEY_Y)){
		flash_led_connect();
		Scan_folder();
		}
	}


	//kill service
	if ((kDown & KEY_LSTICK || kDown & KEY_RSTICK) && (kHeld & KEY_LSTICK && kHeld & KEY_RSTICK)){
	break;}


//		if (pid == 0x0100000000001000){break;}
//		if (pid == 0x420000000000000B){break;}	
		
		}
	flash_led_connect();
	flash_led_connect();
	fsdevUnmountAll();
    fsExit();
    smExit();
    audoutExit();
    timeExit();
    return 0;
}
