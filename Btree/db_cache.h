/**
 * @file db_cache.h Block caching functions
 * 
 * $Id: db_cache.h,v 1.2 2002/04/07 18:29:41 chipx86 Exp $
 *
 * @Copyright (C) 1999-2002 The GNUpdate Project.
 *
 * This library is BFree software; you can redistribute it and/or
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
#ifndef _DB_CACHE_H_
#define _DB_CACHE_H_

// #define MAX_CACHE_SIZE				(4 << 10)
#define MAX_CACHE_SIZE				(1 << 10)
#define CACHE_HASH_SIZE				(MAX_CACHE_SIZE << 3)

/**
 * Adds a block to the cache.
 * 
 * If the block is already in the cache, the reference count will
 * be incremented.
 * 
 * @param db    The database.
 * @param block The block to add to the cache.
 */
void gdbCacheAddBlock(GDatabase *db, GdbBlock *block);

/**
 * Removes a block from the cache.
 *
 * If the block's reference count is greater than 1, the block will
 * stay in the cache and the reference count will be decremented.
 * If the reference count is 1, the block will be removed from the
 * cache.
 * 
 * @param db    The database.
 * @param block The block to remove from the cache.
 *
 * @return The reference count on the block.
 */
unsigned short gdbCacheRemoveBlock(GDatabase *db, GdbBlock *block);

/**
 * Returns a block from the cache.
 *
 * @param db     The database.
 * @param offset The offset of the block.
 *
 * @return The block at @a offset, or @c NULL if it's not in the cache.
 */
GdbBlock *gdbCacheGetBlock(GDatabase *db, offset_t offset);

#endif /* _DB_CACHE_H_ */

