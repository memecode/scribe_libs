/**
 * @file db_blocklist.c Free blocks list
 * 
 * $Id: db_blocklist.c,v 1.7 2002/06/23 10:28:06 chipx86 Exp $
 *
 * @Copyright (C) 1999-2002 The GNUpdate Project.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 */
#include <stdint.h>
#include "db_internal.h"
#include "config.h"

char
gdbGetFreeBlockList(GDatabase *db, GdbFreeBlock **blocks, long *count)
{
	GdbFreeBlock *blockList;
	unsigned long listSize;
	unsigned char *buffer;
	int counter = 0;
	
	if (blocks == NULL || count == NULL)
		return 0;

	*blocks = NULL;
	
	/* Seek to the start of the block list. */
	fseek(db->fp, DB_FREE_BLOCK_LIST_OFFSET /* 16 */, SEEK_SET);

	if (fread(&db->freeBlockCount, sizeof(db->freeBlockCount), 1, db->fp) != 1)
		db->freeBlockCount = 0;

	*count = db->freeBlockCount;

	if (db->freeBlockCount == 0)
		return 0;

	/* Get the total size of the free blocks list. */
	listSize = db->freeBlockCount * (sizeof(short) + sizeof(offset_t));

	/* Allocate the buffer. */
	MEM_CHECK_RET_ZERO(buffer = (unsigned char *)malloc(listSize));
	
	/* Read in the list. */
	if (fread(buffer, listSize, 1, db->fp) != 1)
	{
		fprintf(stderr, _("ERROR: Truncated block list.\n"));
		return 0;
	}

	MEM_CHECK_RET_ZERO(blockList = (GdbFreeBlock *)malloc(db->freeBlockCount *
												 sizeof(GdbFreeBlock)));
		
	
	for (uint32_t i = 0; i < db->freeBlockCount; i++)
	{
		blockList[i].size   = gdbGet16(buffer, &counter);
		blockList[i].offset = gdbGet32(buffer, &counter);
	}

	*blocks = blockList;
	
	free(buffer);
	
	return 1;
}

void
gdbWriteFreeBlockList(GDatabase *db, GdbFreeBlock *blocks, long count)
{
	uint32_t listSize = 0;
	unsigned char *buffer;
	int i, counter = 0;
	
	if (db == NULL || blocks == NULL)
		return;

	/* Get the total size of the list. */
	listSize = (uint32_t) (sizeof(uint32_t) + (count * (sizeof(short) + sizeof(offset_t))));

	/* Allocate the buffer for the block list. */
	MEM_CHECK(buffer = (unsigned char *)malloc(listSize));

	gdbPut32(buffer, &counter, count);

	for (i = 0; i < count; i++)
	{
		gdbPut16(buffer, &counter, blocks[i].size);
		gdbPut32(buffer, &counter, blocks[i].offset);
	}
	
	fseek(db->fp, DB_FREE_BLOCK_LIST_OFFSET, SEEK_SET);

	fwrite(buffer, listSize, 1, db->fp);

	free(buffer);

	fflush(db->fp);
}

void
gdbFreeBlockList(GdbFreeBlock *blocks)
{
	if (blocks == NULL)
		return;
	
	free(blocks);
}

