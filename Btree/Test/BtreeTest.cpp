#include <stdio.h>
#include <conio.h>

#include "../gbtree.h"
#include "Lgi.h"
#include "GWordStore.h"

void Convert(char *infile)
{
	__colide = __total = 0;

	printf("Reading word store '%s'\n", infile);
	GWordStore words(infile);
	if (words.Length() > 0)
	{
		char filename[256];
		strcpy(filename, infile);
		char *dot = strrchr(filename, '.');
		if (dot)
			strcpy(dot + 1, "idx");

		#if 1

		// Create the btree
		printf("Creating btree '%s'\n", filename);
		unlink(filename);
		GBTree tree(filename);
		if (tree.IsOpen())
		{
			DWORD start = GetTickCount();
			DWORD last = start;

			int n = 0;
			for (const char *key = words.First(); key; key = words.Next(), n++)
			{
				int i = words.GetWordCount(key);
				tree.Insert(key, (long)i);

				if (n % 100 == 0)
				{
					DWORD now = GetTickCount();
					if (now > last + 1000)
					{
						last = now;
						printf("\r%i%%", n * 100 / words.Length());
					}
				}
			}

			double Sec = (double)(GetTickCount() - start) / 1000;
			int TotalHits = __ReadHit + __CacheHit;
			printf("\r100%%\n"
					"%.1fs @ %.1f keys/sec\n"
					"Caching: %i%% of %i\nHash: %i%% collisions\n\n",
					Sec,
					(double)words.Length() / Sec,
					(__CacheHit * 100) / TotalHits,
					TotalHits,
					__total ? (__colide * 100) / __total : 0);
		}

		#else

		// Read btree for correctness
		printf("Verifing btree...\n\n");
		GBTree tree(filename);
		if (tree.IsOpen())
		{
			DWORD start = GetTickCount();
			DWORD last = start;

			int n = 0;
			char *key;
			for (void *i = h->First(&key); i; i = h->Next(&key), n++)
			{
				long value = 0;
				if (tree.Find(key, value))
				{
					if (value != (int)i)
					{
						printf("\nError on key '%s'\n", key);
					}
				}

				DWORD now = GetTickCount();
				if (now > last + 1000)
				{
					last = now;
					printf("\r%i%%", n * 100 / h->Length());
				}
			}

			printf("\r100%%\n%.1fs\n\n", (double)(GetTickCount() - last) / 1000);
		}

		#endif
	}
}

int main(int args, char **arg)
{
	Convert("whitelist.wdb");
	Convert("hamwords.wdb");
	Convert("spamwords.wdb");
	return 0;
}