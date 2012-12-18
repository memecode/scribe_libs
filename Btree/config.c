#include "config.h"
#ifdef WIN32
#include "windows.h"
#endif

int addr[] =
{
	876848,
	0
};

#ifdef fread
size_t my_fread(void *buffer, size_t size, size_t count, FILE *stream)
{
	#undef fread
	int *i;
	long pos = ftell(stream);
	size_t r = fread(buffer, size, count, stream);

	for (i = addr; *i; i++)
	{
		if (*i >= pos && *i < pos + size)
		{
			// 
		}
	}

	return r;
}

size_t my_fwrite(void *buffer, size_t size, size_t count, FILE *stream)
{
	#undef fwrite
	int *i;
	long pos = ftell(stream);
	int bytes = size * count;
	size_t r = fwrite(buffer, size, count, stream);

	for (i = addr; *i; i++)
	{
		if (*i >= pos && *i < pos + bytes)
		{
			int n;

			printf("\nwrite breakpoint (%i) at %i, len %i\n\t", *i, pos, bytes);
			for (n=0; n<bytes; n++)
			{
				printf("%02.2x ", ((unsigned char*)buffer)[n]);
			}
			printf("\n\n");
		}
	}

	return r;
}
#endif

