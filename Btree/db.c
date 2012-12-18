/**
 * @file db.c GNUpdate database functions
 *
 * $Id: db.c,v 1.23 2002/06/23 10:28:05 chipx86 Exp $
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
#include "db_internal.h"

static void
__setupDatabase(GDatabase *db)
{
	int bytes;

	db->openBlockCount = 0;
	#if CACHE_SORTED
	bytes = CACHE_HASH_SIZE * sizeof(GdbBlock *);
	MEM_CHECK(db->hash = (GdbBlock **)malloc(bytes));
	memset(db->hash, 0, bytes);
	db->root = 0;
	db->lruStart = 0;
	db->lruEnd = 0;
	#else
	db->openBlockSize  = 16;
	MEM_CHECK(db->openBlocks = (GdbBlock **)malloc(db->openBlockSize *
												   sizeof(GdbBlock *)));
	memset(db->openBlocks, 0, db->openBlockSize * sizeof(GdbBlock *));
	#endif
}

GDatabase *
gdbOpen(const char *filename, GdbType type, PmAccessMode mode)
{
	GDatabase *db;
	FILE      *fp;

	if (filename == NULL || mode == PM_MODE_ERROR)
		return NULL;

	if (mode == PM_MODE_READ_WRITE)
		fp = fopen(filename, "rb+");
	else
		fp = fopen(filename, "rb");

	if (fp == NULL)
	{
		if (mode == PM_MODE_READ_WRITE)
			return gdbCreate(filename, type);
		else
		{
			fprintf(stderr, _("Error: "
					"Unable to open database %s for reading.\n"),
					filename);

			return NULL;
		}
	}

	MEM_CHECK_RET_ZERO(db = (GDatabase *)malloc(sizeof(GDatabase)));
	memset(db, 0, sizeof(GDatabase));

	db->fp = fp;

	if ((gdbReadHeader(db) == 0) || (db->type != type))
	{
		fclose(fp);

		free(db);

		return NULL;
	}

	__setupDatabase(db);

	db->mode     = mode;
	db->filename = strdup(filename);
	db->mainTree = btreeOpen(db, DB_MAIN_TREE_OFFSET);

	return db;
}

void
gdbClose(GDatabase *db)
{
	if (db == NULL)
		return;

	if (db->fp != NULL)
		fclose(db->fp);

	btreeClose(db->mainTree);

	gdbDestroy(db);
}

GDatabase *
gdbCreate(const char *filename, GdbType type)
{
	GDatabase *db;
	FILE      *fp;

	if (filename == NULL)
		return NULL;

	fp = fopen(filename, "wb+");

	if (fp == NULL)
	{
		fprintf(stderr, _("Error: "
				"Unable to open database %s for reading/writing.\n"),
				filename);

		return NULL;
	}

	MEM_CHECK_RET_ZERO(db = (GDatabase *)malloc(sizeof(GDatabase)));
	memset(db, 0, sizeof(GDatabase));

	__setupDatabase(db);

	db->filename = strdup(filename);
	db->type     = type;
	db->fp       = fp;

	gdbWriteHeader(db);

	/* Leave enough room for the free block list. */
	fseek(db->fp, DB_FREE_BLOCK_LIST_OFFSET, SEEK_SET);
	gdbPad(db->fp, DB_FREE_BLOCK_LIST_SIZE);

	db->mainTree = btreeCreate(db, 5);

	return db;
}

GDatabase *
gdbDestroy(GDatabase *db)
{
	if (db == NULL)
		return NULL;

	#if CACHE_SORTED
	#else
	free(db->openBlocks);
	#endif
	free(db->filename);
	free(db);

	return NULL;
}

GdbStatus
gdbAddDataEntry(GDatabase *db, GdbHashTable *table, unsigned short key,
				const void *data, unsigned short size)
{
	if (db == NULL || db->fp == NULL || key == 0 || table == NULL ||
		data == NULL || size == 0)
	{
		return GDB_ERROR;
	}

	htAdd(table, key, data, GDB_HT_RAW, size);

	return GDB_SUCCESS;
}

GdbStatus
gdbAddIndexEntry(GDatabase *db, BTree *tree, const char *key, offset_t offset)
{
	if (db == NULL || tree == NULL || db->fp == NULL || key == NULL ||
		offset == 0)
	{
		return GDB_ERROR;
	}
	
	return btreeInsert(tree, key, offset);
}

GdbStatus
gdbAddTree(GDatabase *db, BTree *tree, const char *key, BTree **newTree)
{
	GdbStatus   status;
	offset_t    offset;
	short       blockSize;
	blocktype_t type;
	
	if (db == NULL || tree == NULL || db->fp == NULL || key == NULL ||
		newTree == NULL)
	{
		return GDB_ERROR;
	}

	*newTree = btreeCreate(db, 5);

	offset = (*newTree)->block->offset;
	blockSize = (*newTree)->block->multiple;
	type = (*newTree)->block->type;

	if (*newTree == NULL)
	{
		*newTree = NULL;

		return GDB_ERROR;
	}

	status = btreeInsert(tree, key, offset);

	if (status == GDB_DUPLICATE)
	{
		/* Return the existing newTree. */
		btreeClose(*newTree);
		gdbFreeBlock(db, offset, type);

		offset = btreeSearch(tree, key);

		if (offset == 0)
		{
			/* I doubt this will ever happen. */
			btreeSetError("Warning: Possible database corruption!");
			return GDB_ERROR;
		}

		*newTree = btreeOpen(db, offset);

		status = GDB_SUCCESS;
	}
	else if (status == GDB_ERROR)
	{
		btreeClose(*newTree);

		fseek(db->fp, offset, SEEK_SET);

#if 0
		gdbPad(db->fp, blockSize);
#endif

		gdbFreeBlock(db, offset, type);

		*newTree = NULL;
	}

	return status;
}

GdbStatus
gdbAddHashTable(GDatabase *db, BTree *tree, const char *key,
				GdbHashTable **newTable)
{
	return GDB_ERROR;
}

