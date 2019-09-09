/**
 * @file db_cache.c Block caching functions
 * 
 * $Id: db_cache.c,v 1.5 2002/06/23 10:28:06 chipx86 Exp $
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

int nextAccess = 1;

#if CACHE_SORTED

#define HASH(o)			(((o) >> 6) % CACHE_HASH_SIZE)

int __colide = 0;
int __total = 1;

#ifdef HASH

#define HASHWRAP(o)		((o) & (CACHE_HASH_SIZE - 1))

int hashVerify(GDatabase *db)
{
	int i;
	for (i=0; i<CACHE_HASH_SIZE; i++)
	{
		if (db->hash[i])
		{
			int h = HASH(db->hash[i]->offset);
			if (h != i)
			{
				int n;
				for (n=1; true; n++)
				{
					int nidx = HASHWRAP(i - n);
					if (db->hash[nidx])
					{
						if (h == nidx)
						{
							break;
						}
					}
					else
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

int hashInsert(GDatabase *db, GdbBlock *block)
{
	int i;
	int h = HASH(block->offset);
	
	for (i=0; i<CACHE_HASH_SIZE; i++)
	{
		int idx = HASHWRAP(h + i);
		if (db->hash[idx] == 0)
		{
			db->hash[idx] = block;
			return true;
		}
		else if (db->hash[idx] == block)
		{
			return true;
		}
	}

	return false;
}

int hashDelete(GDatabase *db, GdbBlock *block)
{
	int i, n;
	int h = HASH(block->offset);

	for (i=0; i<CACHE_HASH_SIZE; i++)
	{
		int idx = HASHWRAP(h + i);
		if (db->hash[idx] == 0)
		{
			return false;
		}
		else if (db->hash[idx] == block)
		{
			int hole = idx;

			/* Remove this actual entry */
			db->hash[idx] = 0;

			/* Bubble down any higher entries */
			for (n=1; true; n++)
			{
				int nidx = HASHWRAP(idx + n);
				if (db->hash[nidx])
				{
					int wrap = nidx < hole;
					int natural_hash = HASH(db->hash[nidx]->offset);
					if (natural_hash != nidx && natural_hash <= (wrap ? hole - CACHE_HASH_SIZE : hole))
					{
						db->hash[hole] = db->hash[nidx];
						db->hash[nidx] = 0;
						hole = nidx;
					}
				}
				else break;
			}

			// hashVerify(db);
			return true;
		}
	}

	return false;
}

#endif

void treeInsert(GDatabase *db, GdbBlock *block)
{
	block->left = 0;
	block->right = 0;

	if (db->root)
	{
		GdbBlock *b = db->root;
		while (b)
		{
			if (b == block)
			{
				return;
			}

			if (block->offset < b->offset)
			{
				if (b->left)
				{
					assert(b->left != b);
					b = b->left;
				}
				else
				{
					b->left = block;
					return;
				}
			}
			else if (block->offset > b->offset)
			{
				if (b->right)
				{
					assert(b->right != b);
					b = b->right;
				}
				else
				{
					b->right = block;
					return;
				}
			}
			else
			{
				assert(0);
				return;
			}
		}
	}
	else
	{
		db->root = block;
	}
}

int verifyNode(GdbBlock *b)
{
	if (b)
	{
		if (b == b->left)
			return 0;
		if (b == b->right)
			return 0;
		if (b->left && b->left->offset > b->offset)
			return 0;
		if (b->right && b->right->offset < b->offset)
			return 0;

		if (!verifyNode(b->left) ||
			!verifyNode(b->right))
			return 0;
	}

	return 1;
}

void treeDelete(GDatabase *db, GdbBlock *block)
{	
	GdbBlock **ptr = &db->root;
	GdbBlock *b = db->root;

	while (b)
	{
		if (b == block)
		{
			break;
		}
		else if (block->offset < b->offset)
		{
			ptr = &b->left;
			b = b->left;
		}
		else
		{
			ptr = &b->right;
			b = b->right;
		}
	}

	if (b == block)
	{
		if (b->left != 0 &&
			b->right != 0)
		{
			GdbBlock *c = b->right, *p = b;
			while (c->left)
			{
				p = c;
				c = c->left;
			}
			if (p == b)
			{
				b->right = c;
			}
			else
			{
				p->left = c->right;
				*ptr = c;
				c->right = b->right;
				c->left = b->left;
			}
		}
		else if (b->left)
		{
			*ptr = b->left;
			b->left = 0;
		}
		else if (b->right)
		{
			*ptr = b->right;
			b->right = 0;
		}
		else
		{
			*ptr = 0;
		}

		block->left = 0;
		block->right = 0;
	}
}

void queInsert(GDatabase *db, GdbBlock *block)
{
	if (db->lruStart)
	{
		block->qnext = db->lruStart;
		block->qprev = 0;
		db->lruStart->qprev = block;
		db->lruStart = block;
	}
	else
	{
		block->qnext = 0;
		block->qprev = 0;
		db->lruStart = db->lruEnd = block;
	}
}

void queDelete(GDatabase *db, GdbBlock *block)
{
	if (block->qprev != 0 ||
		block->qnext != 0)
	{
		if (db->lruStart == block)
		{
			db->lruStart = block->qnext;
			if (block->qnext)
				block->qnext->qprev = 0;
		}
		else if (db->lruEnd == block)
		{
			db->lruEnd = block->qprev;
			if (block->qprev)
				block->qprev->qnext = 0;
		}
		else
		{
			block->qnext->qprev = block->qprev;
			block->qprev->qnext = block->qnext;
		}

		block->qnext = 0;
		block->qprev = 0;
	}
}
#endif

void
gdbCacheAddBlock(GDatabase *db, GdbBlock *block)
{
	// int insertPos = -1;

	if (block->offset == 0)
	{
		fprintf(stderr,
				_("ERROR: Trying to add block to list with offset 0\n"));
		return;
	}

	/* See if it's already in the list. */
	if (block->lastAccess > 0)
		return;
	
	#if CACHE_SORTED

	if (db->openBlockCount >= MAX_CACHE_SIZE)
	{
		/* Need to bump something out of the cache first */
		GdbBlock *least = db->lruEnd;
		while (least)
		{
			if (least->refCount <= 0)
			{
				/* Found a node to bump */
				queDelete(db, least);
				#ifdef HASH
				hashDelete(db, least);
				#else
				treeDelete(db, least);
				#endif
				gdbDestroyBlock(least);
				db->openBlockCount--;
				break;
			}

			least = least->qprev;
		}
	}

	/* Insert in tree so we can find it by offset easily */
	#ifdef HASH
	hashInsert(db, block);
	#else
	treeInsert(db, block);
	#endif

	/* Move the block to the head of the recently used que */
	queDelete(db, block);
	queInsert(db, block);

	/* Update vars */
	db->openBlockCount++;
	block->lastAccess = nextAccess++;
	block->refCount++;
	return;
	
	#else
	
	if (db->openBlockCount >= db->openBlockSize &&
		db->openBlockSize < MAX_CACHE_SIZE)
	{
		/* Grow the cache size */
		GdbBlock **newBlocks;
		int        newSize;

		newSize = 2 * db->openBlockSize;

		MEM_CHECK(newBlocks = (GdbBlock **)malloc(newSize *
												  sizeof(GdbBlock *)));
		memset(newBlocks, 0, newSize * sizeof(GdbBlock *));

		for (i = 0; i < db->openBlockSize; i++)
			newBlocks[i] = db->openBlocks[i];

		free(db->openBlocks);

		db->openBlocks    = newBlocks;
		db->openBlockSize = newSize;
	}

	/* Find a place to put this. */
	if (insertPos < 0)
	{
		for (i = 0; i < db->openBlockSize; i++)
		{
			if (db->openBlocks[i] == NULL)
			{
				db->openBlocks[i] = block;
				db->openBlockCount++;

				block->lastAccess = nextAccess++;
				block->refCount++;
				block->cachePos = i;

				return;
			}
			else
			{
				if (insertPos >= 0)
				{
					if (db->openBlocks[i]->refCount <= 0 &&
						db->openBlocks[i]->lastAccess < db->openBlocks[insertPos]->lastAccess)
					{
						insertPos = i;
					}
				}
				else if (db->openBlocks[i]->refCount <= 0)
				{
					insertPos = i;
				}
			}
		}
	}

	/* No open slots so throw out the oldest block */
	if (insertPos >= 0)
	{
		/* Remove and existing block and destroy it */
		GdbBlock *del = db->openBlocks[insertPos];
		if (del)
		{
			db->openBlocks[insertPos] = 0;
			del->cachePos = -1;
			gdbDestroyBlock(del);
		}

		/* Insert new block */
		db->openBlocks[insertPos] = block;
		db->openBlockCount++;

		block->lastAccess = nextAccess++;
		block->refCount++;
		block->cachePos = insertPos;

		return;
	}

	#endif

	// fprintf(stderr, _("** WARNING: Couldn't place the open block in the list!\n"));
}

unsigned short
gdbCacheRemoveBlock(GDatabase *db, GdbBlock *block)
{
	if (block->offset == 0)
	{
		btreeSetError("ERROR: Trying to remove block from list with offset 0");
		return 0;
	}
	
	if (db->openBlockCount == 0)
	{
		btreeSetError("** WARNING: db->openBlockCount == 0!");
		return 0;
	}

	#if CACHE_SORTED

	if (block->lastAccess >= 0)
	{
		if (block->refCount > 0)
		{
			block->refCount--;
		}

		return 1;
	}

	#else

	if (block->cachePos < db->openBlockSize)
	{
		if (db->openBlocks[block->cachePos] == block)
		{
			if (block->refCount > 0)
			{
				block->refCount--;
			}

			return 1;
		}
	}

	#endif

	/*
	fprintf(stderr, _("** WARNING: No open block found of offset %ld!\n"),
			block->offset);
	*/

	return 0;
}

GdbBlock *
gdbCacheGetBlock(GDatabase *db, offset_t offset)
{
	#if CACHE_SORTED

	#ifdef HASH

	int h = HASH(offset), i;

	for (i=0; i<CACHE_HASH_SIZE; i++)
	{
		int idx = HASHWRAP(h + i);

		__total++;

		if (db->hash[idx])
		{
			if (db->hash[idx]->offset == offset)
			{
				return db->hash[idx];
			}
		}
		else
		{
			return 0;
		}

		__colide++;
	}

	#else

	GdbBlock *b = db->root;
	while (b)
	{
		if (b->offset == offset)
			return b;
		else if (offset < b->offset)
			b = b->left;
		else
			b = b->right;
	}
	
	#endif
	
	#else

	int i;

	for (i = 0; i < db->openBlockSize; i++)
	{
		if (db->openBlocks[i] != NULL &&
			db->openBlocks[i]->offset == offset)
		{
			db->openBlocks[i]->lastAccess = nextAccess++;
			db->openBlocks[i]->refCount++;

			return db->openBlocks[i];
		}
	}

	#endif
	
	return NULL;
}


