/**
 * @file db_blocks.c Blocks management
 * 
 * $Id: db_blocks.c,v 1.27 2002/06/23 10:28:06 chipx86 Exp $
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
#include "config.h"

typedef struct
{
	unsigned short multiple;

	void *(*readBlock)(GdbBlock *block, const char *buffer, void *extra);
	void (*writeBlock)(GdbBlock *block, char **buffer, unsigned long *size);

	void *(*create)(GdbBlock *block, void *extra);
	void (*destroy)(void *data);

} GdbBlockTypeInfo;

static GdbBlockTypeInfo blockTypeInfo[] =
{
	/** Raw data block */
	{ 4, NULL, NULL, NULL, NULL },

	/** B+Tree header block */
	{ 32, btreeReadHeader, btreeWriteHeader, btreeCreateHeader,
	      btreeDestroyHeader },

	/** B+Tree node block */
	{
		256,
		btreeReadNodeBlock,
		btreeWriteNodeBlock,
		btreeCreateNodeBlock,
		btreeDestroyNodeBlock
	},

	/** Hashtable block */
	{ 64, htReadBlock, htWriteBlock, htCreateBlock, htDestroyBlock },

	/** Offset List block */
	{ 32, olReadBlock, olWriteBlock, olCreateBlock, olDestroyBlock }
};

static int
__blockCompare(const void *a, const void *b)
{
	const GdbFreeBlock *block1 = (const GdbFreeBlock *)a;
	const GdbFreeBlock *block2 = (const GdbFreeBlock *)b;

	if (block1->size   > block2->size)   return  1;
	if (block1->size   < block2->size)   return -1;
	if (block1->offset > block2->offset) return  1;
	if (block1->offset < block2->offset) return -1;
	
	return 0;
}

static int
__offsetCompare(const void *a, const void *b)
{
	offset_t o1 = *(offset_t *)a;
	offset_t o2 = *(offset_t *)b;

	if (o1 < o2) return -1;
	if (o1 > o2) return  1;
	
	return  0;
}

GdbBlock *
gdbNewBlock(GDatabase *db, blocktype_t blockType, void *extra)
{
	GdbBlock *block;
	blocktype_t typeIndex;

	if (db == NULL || !GDB_VALID_BLOCK_TYPE(blockType))
		return NULL;

	MEM_CHECK_RET_ZERO(block = (GdbBlock *)malloc(sizeof(GdbBlock)));
	memset(block, 0, sizeof(GdbBlock));

	block->type       = blockType;
	block->db         = db;
	#if CACHE_SORTED
	block->left       = 0;
	block->right      = 0;
	#else
	block->cachePos   = -1;
	#endif
	block->refCount   = 0;
	block->lastAccess = 0;

	GDB_SET_DIRTY(block);

	typeIndex = blockType - 1;

	block->multiple = blockTypeInfo[typeIndex].multiple;

	if (blockTypeInfo[typeIndex].create != NULL)
	{
		block->detail = blockTypeInfo[typeIndex].create(block, extra);
	}

	return block;
}

void
gdbDestroyBlock(GdbBlock *block)
{
	blocktype_t typeIndex;
	
	if (block == NULL)
		return;

	if (gdbCacheRemoveBlock(block->db, block) > 0)
		return;

	typeIndex = block->type - 1;
	
	if (block->detail != NULL)
	{
		if (blockTypeInfo[typeIndex].destroy != NULL)
			blockTypeInfo[typeIndex].destroy(block->detail);
		else
			free(block->detail);
	}

	if (GDB_IS_DIRTY(block))
	{
		btreeSetError("WARNING: Dirty block has not been written to disk! Offset: %ld", block->offset);
		return;
	}

	if (block->chain != NULL)
		free(block->chain);

	free(block);
}

int __CacheHit = 0;
int __ReadHit = 0;

GdbBlock *
gdbReadBlockHeader(GDatabase *db, offset_t offset, blocktype_t blockType)
{
	GdbBlock *block;
	char header[GDB_BLOCK_HEADER_SIZE];
	int counter = 0;
	blocktype_t typeIndex;

	if (db == NULL || !GDB_VALID_OFFSET(offset) ||
		(blockType != GDB_BLOCK_ANY && !GDB_VALID_BLOCK_TYPE(blockType)))
	{
		return NULL;
	}

	/* See if the block is cached. */
	if ((block = gdbCacheGetBlock(db, offset)) != NULL)
	{
		if (blockType == GDB_BLOCK_ANY || blockType == block->type)
		{
			__CacheHit++;
			return block;
		}
		else
			return NULL;
	}

	__ReadHit++;

	/* Seek to the offset of the block. */
	fseek(db->fp, offset, SEEK_SET);
	
	if (fread(header, GDB_BLOCK_HEADER_SIZE, 1, db->fp) != 1)
	{
		return NULL;
	}

	/* Allocate memory for the block. */
	MEM_CHECK_RET_ZERO(block = (GdbBlock *)malloc(sizeof(GdbBlock)));
	memset(block, 0, sizeof(GdbBlock));

	block->db = db;

	/* Store the info from the header. */
	block->type = gdbGet8(header, &counter);

	/* Make sure the type is valid. */
	if (!GDB_VALID_BLOCK_TYPE(block->type) ||
		(blockType != GDB_BLOCK_ANY && blockType != block->type))
	{
		free(block);

		return NULL;
	}

	typeIndex = block->type - 1;

	block->offset = offset;
	
	block->multiple = blockTypeInfo[typeIndex].multiple;
	
	block->dataSize = gdbGet32(header, &counter);
	block->flags    = gdbGet16(header, &counter);
	block->next     = gdbGet32(header, &counter);
	block->listNext = gdbGet32(header, &counter);

	GDB_CLEAR_DIRTY(block);

	gdbCacheAddBlock(block->db, block);

	return block;
}

void
gdbWriteBlockHeader(GdbBlock *block)
{
	GDatabase *db;
	char header[GDB_BLOCK_HEADER_SIZE];
	int   counter = 0;

	if (block == NULL || !GDB_IS_DIRTY(block))
		return;

	db = block->db;

	if (block->offset == 0)
		return;

	/* Write the header to a buffer. */
	gdbPut8(header,  &counter, block->type);
	gdbPut32(header, &counter, block->dataSize);
	gdbPut16(header, &counter, block->flags);
	gdbPut32(header, &counter, block->next);
	gdbPut32(header, &counter, block->listNext);

	/* Write the header to disk. */
	fseek(db->fp, block->offset, SEEK_SET);

	fwrite(header, GDB_BLOCK_HEADER_SIZE, 1, db->fp);

	fflush(db->fp);

	GDB_CLEAR_DIRTY(block);

	if (!block->lastAccess)
		gdbCacheAddBlock(block->db, block);
}

GdbBlock *
gdbReadBlock(GDatabase *db, offset_t offset, blocktype_t blockType,
			 void *extra)
{
	GdbBlock     *block;
	char         *buffer;
	unsigned long pos, i;
	blocktype_t   typeIndex;

	if (db == NULL || !GDB_VALID_OFFSET(offset) |
		(blockType != GDB_BLOCK_ANY && !GDB_VALID_BLOCK_TYPE(blockType)))
	{
		return NULL;
	}

	if ((block = gdbCacheGetBlock(db, offset)) != NULL)
	{
		if (blockType == GDB_BLOCK_ANY || blockType == block->type)
		{
			__CacheHit++;
			return block;
		}
		else
			return NULL;
	}

	__ReadHit++;

	block = gdbReadBlockHeader(db, offset, blockType);

	if (block == NULL)
	{
		fprintf(stderr, _("ERROR: Unable to read block at %ld\n"), offset);
		fprintf(stderr, _("ERROR: `%s'\n"), db->filename);
		return NULL;
	}

	/* Get the number of needed blocks. */
	block->chainCount =
		gdbGetNeededBlockCount(block->dataSize, block->multiple);

	/* Build the chain array. */
	MEM_CHECK_RET_ZERO(block->chain = (offset_t *)malloc(block->chainCount *
												sizeof(offset_t)));
	memset(block->chain, 0, block->chainCount * sizeof(offset_t));

	block->chain[0] = offset;
	
	typeIndex = block->type - 1;

	/* Create the buffer. */
	MEM_CHECK_RET_ZERO(buffer = (char *)malloc(block->dataSize));
		
	/* Read in the first block. */
	if (fread(buffer,
			  (block->dataSize < block->multiple - GDB_BLOCK_HEADER_SIZE ?
			   block->dataSize : block->multiple - GDB_BLOCK_HEADER_SIZE),
			  1, db->fp) != 1)
	{
		btreeSetError("ERROR: Unable to read from %s at offset %ld, %ld bytes",
		        db->filename, ftell(db->fp),
			    (block->dataSize < block->multiple - GDB_BLOCK_HEADER_SIZE ?
			    block->dataSize : block->multiple - GDB_BLOCK_HEADER_SIZE));
		return 0;
	}

	pos = block->multiple - GDB_BLOCK_HEADER_SIZE;

	if (block->next != 0)
	{
		offset_t nextOffset = block->next;
		offset_t prevOffset = block->offset;
		unsigned short blockDataSize = block->multiple - sizeof(offset_t);

		block->chain[1] = nextOffset;

		i = 2;

		/* Read in any overflow blocks. */
		while (nextOffset != 0)
		{
			if (prevOffset + block->multiple != nextOffset)
				fseek(db->fp, nextOffset, SEEK_SET);
			
			prevOffset = nextOffset;
			
			fread(&nextOffset, sizeof(offset_t), 1, db->fp);
			
			if (prevOffset == nextOffset)
			{
				fprintf(stderr, "[%s]\n", db->filename);
				fprintf(stderr, _("FATAL: Infinite loop detected in database "
								  "blocks!\n"));
				return NULL;
			}
			
			if (i < block->chainCount)
				block->chain[i++] = nextOffset;
			
			fread(buffer + pos, 1,
				  (block->dataSize - pos < blockDataSize ?
				   block->dataSize - pos : blockDataSize),
				  db->fp);

			pos += blockDataSize;
		}
	}

	/* See if there is a read function assigned. */
	if (blockTypeInfo[typeIndex].readBlock != NULL)
	{
		/* Call the specific block type's read function. */
		block->detail =
			blockTypeInfo[typeIndex].readBlock(block, buffer, extra);

		free(buffer);
	}
	else
	{
		/* Just use the buffer as the detailed info. */
		block->detail = buffer;
	}
	
	return block;
}

int
gdbWriteBlock(GdbBlock *block)
{
	GDatabase    *db;
	char         *buffer;
	offset_t     *oldChain;
	blocktype_t   typeIndex;
	unsigned long oldChainCount, oldDataSize;
	unsigned long  i, pos;
	
	if (block == NULL || !GDB_IS_DIRTY(block))
	{
	    assert(!"Param error");
		return 0;
	}

	/* Set a couple of vars we'll be using. */
	db        = block->db;
	typeIndex = block->type - 1;

	/* Save the old data. */
	oldDataSize   = block->dataSize;
	oldChainCount = block->chainCount;
	oldChain      = block->chain;

	/* See if there is a write function assigned. */
	if (blockTypeInfo[typeIndex].writeBlock != NULL)
	{
		/* Write the block info to a buffer. */
		blockTypeInfo[typeIndex].writeBlock(block, &buffer, &block->dataSize);
	}
	else
	{
		buffer = (char *)block->detail;
	}

	if (buffer == NULL)
	{
	    assert(!"Buffer should never be NULL.");
		return 0;
	}

	/* Get the number of needed blocks. */
	block->chainCount = gdbGetNeededBlockCount(block->dataSize,
											   block->multiple);

	if (oldChainCount == 0)
	{
		/* Reserve new blocks. */
		block->chain = gdbReserveBlockChain(db, (unsigned short) block->chainCount,
											block->type);
	}
	else if (block->chainCount < oldChainCount)
	{
		/* The number of needed blocks is shorter than before. */
		MEM_CHECK_RET_ZERO(block->chain = (offset_t *)malloc(block->chainCount *
													sizeof(offset_t)));
		memcpy(block->chain, oldChain, block->chainCount * sizeof(offset_t));
	}
	else if (block->chainCount > oldChainCount)
	{
		offset_t *newChain;
		unsigned long j;
		
		/* The number of needed blocks is longer than before. */
		MEM_CHECK_RET_ZERO(block->chain = (offset_t *)malloc(block->chainCount *
													sizeof(offset_t)));
			
		newChain = gdbReserveBlockChain(db, (unsigned short) (block->chainCount - oldChainCount),
										block->type);
		
		memcpy(block->chain, oldChain, oldChainCount * sizeof(offset_t));
		
		for (i = oldChainCount, j = 0; i < block->chainCount; i++, j++)
			block->chain[i] = newChain[j];

		free(newChain);
	}

	/*
	 * Set the offset and next block, if this spills over into
	 * additional blocks.
	 */
	block->offset = block->chain[0];

	if (block->chainCount > 1)
		block->next = block->chain[1];
	else
		block->next = 0;
	
	/* Write the first block header */
	gdbWriteBlockHeader(block);

	/* Write the first block. */
	fwrite(	buffer,
			(block->dataSize < block->multiple - GDB_BLOCK_HEADER_SIZE ?
			block->dataSize : block->multiple - GDB_BLOCK_HEADER_SIZE),
			1,
			db->fp);

	if (block->dataSize < block->multiple - GDB_BLOCK_HEADER_SIZE /* 15 */)
	{
		gdbPad(db->fp, block->multiple - GDB_BLOCK_HEADER_SIZE -
			   block->dataSize);
	}
	else
	{
		char *blockBuffer;
		
		MEM_CHECK_RET_ZERO(blockBuffer = (char *)malloc(block->multiple));

		pos = block->multiple - GDB_BLOCK_HEADER_SIZE;
		
		/* Write any overflow blocks. */
		for (i = 1; i < block->chainCount; i++)
		{
			offset_t nextOffset;
			unsigned long relPos;
			
			nextOffset = ((i + 1 < block->chainCount) ?
						  block->chain[i + 1] : 0);

			relPos = block->dataSize - pos;
			
			/* Reset the block buffer. */
			memset(blockBuffer, 0, block->multiple);

			/* Write to it. */
			memcpy(blockBuffer, &nextOffset, sizeof(offset_t));
			memcpy(blockBuffer + sizeof(offset_t), buffer + pos,
				   (relPos < block->multiple - sizeof(offset_t) ?
					relPos : block->multiple - sizeof(offset_t)));
			
			/* Write the block buffer. */
			if (block->chain[i - 1] + block->multiple != block->chain[i])
				fseek(db->fp, block->chain[i], SEEK_SET);

			fwrite(blockBuffer, 1, block->multiple, db->fp);

			pos += block->multiple - sizeof(offset_t);
		}

		free(blockBuffer);
	}

	if (oldChainCount != 0)
	{
		/* If the chain shrunk, free up the discarded blocks. */
		if (block->chainCount < oldChainCount)
		{
			gdbFreeBlockChain(db, &oldChain[block->chainCount],
							  (unsigned short) (oldChainCount - block->chainCount), block->type);
		}
		
		if (oldChainCount != block->chainCount)
			free(oldChain);
	}

	fflush(db->fp);

	if (buffer != block->detail)
		free(buffer);
		
	return 1;
}

offset_t *
gdbReserveBlockChain(GDatabase *db, unsigned short count,
					 blocktype_t blockType)
{
	GdbFreeBlock  *freeBlocks, *newFreeBlocks;
	offset_t      *chain;
	offset_t       offset;
	unsigned short blockSize;
	long           blockCount, fillCount, newListCount;
	long           i, j, result;

	if (db == NULL || count == 0 || !GDB_VALID_BLOCK_TYPE(blockType))
		return NULL;

	/* Get the block size for this type. */
	blockSize = blockTypeInfo[blockType - 1].multiple;

	/* Create the chain. */
	MEM_CHECK_RET_ZERO(chain = (offset_t *)malloc(count * sizeof(offset_t)));

	/* Lock the free block list. */
	gdbLockFreeBlockList(db, DB_WRITE_LOCK);

	/* Get the free block list. */
	result = gdbGetFreeBlockList(db, &freeBlocks, &blockCount);

	if (result == 0)
	{
		gdbUnlockFreeBlockList(db);
		gdbFreeBlockList(freeBlocks);

		fseek(db->fp, 0L, SEEK_END);
		offset = ftell(db->fp);

		/* Fill in the chain with the reserved offsets. */
		for (i = 0; i < count; i++)
			chain[i] = offset + (i * blockSize);

		gdbPad(db->fp, count * blockSize);

		return chain;
	}

	fillCount = 0;
	j = 0;

	/* Create the new array of free blocks. */
	MEM_CHECK_RET_ZERO(newFreeBlocks = (GdbFreeBlock *)malloc(blockCount *
													 sizeof(GdbFreeBlock)));
	memset(newFreeBlocks, 0, blockCount * sizeof(GdbFreeBlock));
	
	for (i = 0; i < blockCount; i++)
	{
		if (fillCount < count && freeBlocks[i].size == blockSize)
		{
			chain[fillCount++] = freeBlocks[i].offset;
		}
		else
		{
			newFreeBlocks[j].offset = freeBlocks[i].offset;
			newFreeBlocks[j].size   = freeBlocks[i].size;

			j++;
		}
	}

	newListCount = j;
	
	if (fillCount != count)
	{
		if (fillCount > 0)
			gdbWriteFreeBlockList(db, newFreeBlocks, newListCount);

		gdbUnlockFreeBlockList(db);

		gdbFreeBlockList(newFreeBlocks);
		gdbFreeBlockList(freeBlocks);

		fseek(db->fp, 0L, SEEK_END);
		offset = ftell(db->fp);

		/* Fill in the chain with the reserved offsets. */
		for (i = fillCount, j = 0; i < count; i++, j++)
			chain[i] = offset + (j * blockSize);

		gdbPad(db->fp, (count - fillCount) * blockSize);

		qsort(chain, count, sizeof(offset_t), __offsetCompare);
		
		return chain;
	}

	/* Write the new list to disk. */
	gdbWriteFreeBlockList(db, newFreeBlocks, newListCount);

	/* Unlock the list. */
	gdbUnlockFreeBlockList(db);

	/* Free up the memory for the lists. */
	gdbFreeBlockList(newFreeBlocks);
	gdbFreeBlockList(freeBlocks);

	/* Sort it. */
	qsort(chain, count, sizeof(offset_t), __offsetCompare);

	return chain;
}

void
gdbFreeBlockChain(GDatabase *db, offset_t *chain, unsigned short count,
				  blocktype_t blockType)
{
	GdbFreeBlock  *freeBlocks;
	GdbFreeBlock  *tempBlocks;
	unsigned short blockSize;
	long           blockCount;
	long           i, j, result;

	if (db == NULL || chain == NULL || count == 0 ||
		!GDB_VALID_BLOCK_TYPE(blockType))
	{
		return;
	}

	/* Get the block size for this type. */
	blockSize = blockTypeInfo[blockType - 1].multiple;

	/* Lock the free block list. */
	gdbLockFreeBlockList(db, DB_WRITE_LOCK);

	/* Get the free block list. */
	result = gdbGetFreeBlockList(db, &freeBlocks, &blockCount);

	if (blockCount == 0)
	{
		/* Block list is empty. */
		MEM_CHECK(freeBlocks = (GdbFreeBlock *)malloc(count *
													  sizeof(GdbFreeBlock)));

		for (i = 0; i < count; i++)
		{
			freeBlocks[i].offset = chain[i];
			freeBlocks[i].size   = blockSize;
		}

		gdbWriteFreeBlockList(db, freeBlocks, count);

		gdbUnlockFreeBlockList(db);
		
		gdbFreeBlockList(freeBlocks);

		return;
	}

	/* We're going to add the block to the list by re-creating the list. */
	tempBlocks = freeBlocks;
	
	MEM_CHECK(freeBlocks = (GdbFreeBlock *)malloc((blockCount + count) *
												  sizeof(GdbFreeBlock)));
	memcpy(freeBlocks, tempBlocks, blockCount * sizeof(GdbFreeBlock));

	for (i = blockCount, j = 0; i < blockCount + count; i++, j++)
	{
		freeBlocks[i].offset = chain[j];
		freeBlocks[i].size   = blockSize;
	}

	gdbFreeBlockList(tempBlocks);

	blockCount += count;

	qsort(freeBlocks, blockCount, sizeof(GdbFreeBlock), __blockCompare);

	gdbWriteFreeBlockList(db, freeBlocks, blockCount);
	gdbFreeBlockList(freeBlocks);

	gdbUnlockFreeBlockList(db);
}

offset_t
gdbReserveBlock(GDatabase *db, blocktype_t blockType)
{
	offset_t *chain;
	offset_t  offset;
	
	if (db == NULL || !GDB_VALID_BLOCK_TYPE(blockType))
		return 0;

	chain = gdbReserveBlockChain(db, 1, blockType);

	if (chain == NULL)
		return 0;

	offset = chain[0];

	free(chain);

	return offset;
}

void
gdbFreeBlock(GDatabase *db, offset_t offset, blocktype_t blockType)
{
	if (db == NULL || !GDB_VALID_OFFSET(offset) ||
		!GDB_VALID_BLOCK_TYPE(blockType))
	{
		return;
	}

	gdbFreeBlockChain(db, &offset, 1, blockType);
}

unsigned long
gdbGetNeededBlockCount(unsigned long dataSize, short multiple)
{
	unsigned long count, i;

	if (dataSize == 0 || multiple == 0)
		return 0;

	dataSize += GDB_BLOCK_HEADER_SIZE;

	if (dataSize == multiple)
		return 1;

	count = 1;

	for (i = multiple; i < dataSize; i += multiple - sizeof(offset_t))
		count++;

	return count;
}
