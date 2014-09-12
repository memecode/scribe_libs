#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "dirent.h"

struct DIR
{
	HANDLE Hnd;
	WIN32_FIND_DATAA Data;
	dirent Entry;
	bool First;
};

DIR *opendir(const char *file)
{
	DIR *d = new DIR;
	if (!d || !file)
		return d;
	
	char all[MAX_PATH];
	sprintf_s(all, sizeof(all), "%s\\*.*", file);
	d->Hnd = FindFirstFileA(all, &d->Data);
	if (d->Hnd == INVALID_HANDLE_VALUE)
	{
		delete d;
		return NULL;
	}
	
	d->First = true;
	return d;
}

int closedir(DIR *d)
{
	if (!d)
		return -1;
	
	FindClose(d->Hnd);
	delete d;
	return 0;
}

struct dirent *readdir(DIR *d)
{
	if (d->First)
	{
		d->First = false;
	}
	else
	{
		if (!FindNextFileA(d->Hnd, &d->Data))
			return NULL;
	}
	
	d->Entry.d_ino = 0;
	d->Entry.d_off = 0;
	d->Entry.d_reclen = sizeof(d->Entry);
	d->Entry.d_type = d->Data.dwFileAttributes;
	strcpy_s(d->Entry.d_name, sizeof(d->Entry.d_name), d->Data.cFileName);
	
	return &d->Entry;
}


