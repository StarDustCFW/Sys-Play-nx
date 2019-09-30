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
