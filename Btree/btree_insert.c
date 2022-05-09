/**
 * @file btree_insert.c Insertion functions
 * 
 * $Id: btree_insert.c,v 1.12 2002/04/07 18:29:40 chipx86 Exp $
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

int __verifyNode(BTreeNode *rootNode)
{
	#ifdef _DEBUG

	int i;
	int as = 1;
	
	if (rootNode == 0)
	{
		if (as) assert(0);
		return 0;
	}

	if (rootNode->keyCount)
	{
		if (rootNode->keys == 0)
		{
			if (as) assert(0);
			return 0;
		}

		if (rootNode->keySizes == 0)
		{
			if (as) assert(0);
			return 0;
		}
	}

	for (i=0; i<rootNode->keyCount; i++)
	{
		size_t len;

		if (rootNode->keys[i] == 0)
		{
			if (as) assert(0);
			return 0;
		}

		len = strlen(rootNode->keys[i]);
		if (len != rootNode->keySizes[i] - 1)
		{
			if (as) assert(0);
			return 0;
		}
	}

	#endif

	return 1;
}

static char
__splitNode(BTree *tree, BTreeNode *rootNode, char **key,
			offset_t *filePos, char *split)
{
	char      *temp1, *temp2;
	BTreeNode *tempNode;
	offset_t   offset1 = 0, offset2;
	unsigned short tempSize1, tempSize2;
	int        i, j, div;

	__verifyNode(rootNode);

	for (i = 0;
		 i < (tree->order - 1) && StringCompare(*key, rootNode->keys[i]) > 0;
		 i++)
		;

	if (i < (tree->order - 1) && StringCompare(*key, rootNode->keys[i]) == 0)
	{
		*split = 0;
		return 0;
	}

	*split = 1;
	
	if (i < (tree->order - 1))
	{
		temp1                 = rootNode->keys[i];
		tempSize1             = rootNode->keySizes[i];
		rootNode->keys[i]     = strdup(*key);
		rootNode->keySizes[i] = (unsigned short) (strlen(*key) + 1);
		j = i;

		for (i++; i < (tree->order - 1); i++)
		{
			temp2     = rootNode->keys[i];
			tempSize2 = rootNode->keySizes[i];
			
			rootNode->keys[i]     = temp1;
			rootNode->keySizes[i] = tempSize1;
			
			temp1     = temp2;
			tempSize1 = tempSize2;
		}

		if (!BTREE_IS_LEAF(rootNode))
			j++;

		offset1 = rootNode->children[j];
		rootNode->children[j] = *filePos;
		
		for (j++; j <= (tree->order - 1); j++)
		{
			offset2 = rootNode->children[j];
			rootNode->children[j] = offset1;
			offset1 = offset2;
		}
	}
	else
	{
		temp1     = strdup(*key);
		tempSize1 = (unsigned short) (strlen(temp1) + 1);

		if (BTREE_IS_LEAF(rootNode))
		{
			offset1 = rootNode->children[tree->order - 1];
			rootNode->children[tree->order - 1] = *filePos;
		}
		else
			offset1 = *filePos;
	}

	if (BTREE_IS_LEAF(rootNode))
		div = (int)((tree->order + 1) / 2) - 1;
	else
		div = (int)(tree->order / 2);

	free(*key);
	*key = strdup(rootNode->keys[div]);
	
	tempNode           = btreeNewNode(tree);
	tempNode->keyCount = tree->order - 1 - div;

	if (BTREE_IS_LEAF(rootNode))
		BTREE_SET_LEAF(tempNode);

	i = div + 1;

	for (j = 0; j < tempNode->keyCount - 1; j++, i++)
	{
		tempNode->keys[j]     = rootNode->keys[i];
		tempNode->keySizes[j] = rootNode->keySizes[i];
		tempNode->children[j] = rootNode->children[i];

		rootNode->keys[i]     = NULL;
		rootNode->keySizes[i] = 0;
		rootNode->children[i] = 0;
	}

	tempNode->keys[j]         = temp1;
	tempNode->keySizes[j]     = tempSize1;
	tempNode->children[j]     = rootNode->children[i];
	rootNode->children[i]     = 0;
	tempNode->children[j + 1] = offset1;

	__verifyNode(tempNode);

	*filePos = btreeWriteNode(tempNode);

	if (BTREE_IS_LEAF(rootNode))
	{
		rootNode->keyCount = div + 1;
		rootNode->children[(int)rootNode->keyCount] = *filePos;
	}
	else
	{
		rootNode->keyCount = div;

		free(rootNode->keys[(int)rootNode->keyCount]);
		rootNode->keys[(int)rootNode->keyCount]     = NULL;
		rootNode->keySizes[(int)rootNode->keyCount] = 0;
	}

	GDB_SET_DIRTY(rootNode->block);

	__verifyNode(rootNode);

	btreeWriteNode(rootNode);

	btreeDestroyNode(tempNode);

	return 1;
}

static char
__addKey(BTree *tree, BTreeNode *rootNode, char **key, offset_t *filePos, char *split)
{
	char     *temp1, *temp2;
	offset_t  offset1, offset2;
	unsigned short tempSize1, tempSize2;
	int       i, j;

	__verifyNode(rootNode);

	*split = 0;

	for (i = 0;
		 i < rootNode->keyCount && StringCompare(*key, rootNode->keys[i]) > 0;
		 i++)
		;

	if (i < rootNode->keyCount && StringCompare(*key, rootNode->keys[i]) == 0)
	{
	    /* Node already exists, update the value and write it out.... */
	    rootNode->children[i] = *filePos;
	    GDB_SET_DIRTY(rootNode->block);
	    btreeWriteNode(rootNode);
		return 1;
	}

	rootNode->keyCount++;

	if (i < rootNode->keyCount)
	{
		temp1     = rootNode->keys[i];
		tempSize1 = rootNode->keySizes[i];

		rootNode->keys[i]     = strdup(*key);
		assert(rootNode->keys[i]);

		rootNode->keySizes[i] = (unsigned short) (strlen(*key) + 1);
		
		j = i;
		
		for (i++; i < rootNode->keyCount; i++)
		{
			temp2     = rootNode->keys[i];
			tempSize2 = rootNode->keySizes[i];

			rootNode->keys[i]     = temp1;
			assert(rootNode->keys[i]);

			rootNode->keySizes[i] = tempSize1;

			temp1     = temp2;
			tempSize1 = tempSize2;
		}

		if (!BTREE_IS_LEAF(rootNode))
			j++;

		offset1 = rootNode->children[j];
		rootNode->children[j] = *filePos;

		for (j++; j <= rootNode->keyCount; j++)
		{
			offset2 = rootNode->children[j];
			rootNode->children[j] = offset1;
			offset1 = offset2;
		}
	}
	else
	{
		rootNode->keys[i]     = strdup(*key);
		assert(rootNode->keys[i]);

		rootNode->keySizes[i] = (unsigned short) (strlen(*key) + 1);
	
		if (BTREE_IS_LEAF(rootNode))
		{
			rootNode->children[i + 1] = rootNode->children[i];
			rootNode->children[i] = *filePos;
		}
		else
		{
			rootNode->children[i + 1] = *filePos;
		}
	}

	GDB_SET_DIRTY(rootNode->block);

	assert(__verifyNode(rootNode));

	btreeWriteNode(rootNode);

	return 1;
}

static char
__insertKey(BTree *tree, offset_t rootOffset, char **key,
			offset_t *filePos, char *split)
{
	char success = 0, error = -1;
	BTreeNode *rootNode;

	if (rootOffset < DB_HEADER_BLOCK_SIZE)
	{
		btreeSetError("ERROR: rootOffset = %ld in __insertKey('%s')", rootOffset, *key);
		return error;
	}

	rootNode = btreeReadNode(tree, rootOffset);
	if (!rootNode)
		return error;

	if (BTREE_IS_LEAF(rootNode))
	{
		if (rootNode->keyCount < (tree->order - 1))
		{
			success = __addKey(tree, rootNode, key, filePos, split);
		}
		else
		{
			success = __splitNode(tree, rootNode, key, filePos, split);
		}

		btreeDestroyNode(rootNode);

		return success;
	}
	else
	{
		/* Internal node. */
		int i;

		for (i = 0;
			 i < rootNode->keyCount && StringCompare(*key, rootNode->keys[i]) > 0;
			 i++)
			;
		
		__verifyNode(rootNode);

		success = __insertKey(tree, rootNode->children[i], key, filePos,
							  split);

		__verifyNode(rootNode);
	}

	if (success == 1 && *split == 1)
	{
		if (rootNode->keyCount < (tree->order - 1))
			__addKey(tree, rootNode, key, filePos, split);
		else
			__splitNode(tree, rootNode, key, filePos, split);
	}

	btreeDestroyNode(rootNode);
	
	return success;
}

GdbStatus
btreeInsert(BTree *tree, const char *key, offset_t filePos)
{
	char  success, split;
	char *newKey;
	
	if (tree == NULL || key == NULL || // filePos == 0 ||
		tree->block->db->mode == PM_MODE_READ_ONLY)
	{
		return GDB_ERROR;
	}
	
	if (tree->block->db->mode == PM_MODE_TEST)
	{
		if (btreeSearch(tree, key) != 0)
			return GDB_DUPLICATE;

		return GDB_SUCCESS;
	}

	newKey = strdup(key);
	
	success = 0;
	split = 0;

	tree->_insFilePos = filePos;
	
	/* Read in the tree data. */
	tree->root     = btreeGetRootNode(tree);
	tree->leftLeaf = btreeGetLeftLeaf(tree);
	tree->size     = btreeGetTreeSize(tree);

	if (tree->root != 0)
	{
		success = __insertKey(tree, tree->root, &newKey, &tree->_insFilePos,
							  &split);

		if (success == 0)
		{
			free(newKey);
			return GDB_DUPLICATE;
		}
	}

	btreeSetTreeSize(tree, tree->size + 1);

	if (tree->root == 0 || split == 1)
	{
		BTreeNode *node = btreeNewNode(tree);

		node->keys[0]     = strdup(newKey);
		assert(node->keys[0]);

		node->keySizes[0] = (unsigned short) (strlen(newKey) + 1);
		node->keyCount    = 1;

		if (tree->root == 0)
		{
			node->children[0] = tree->_insFilePos;
			BTREE_SET_LEAF(node);

			btreeWriteNode(node);

			btreeSetLeftLeaf(tree, node->block->offset);
		}
		else
		{
			node->children[0] = tree->root;
			node->children[1] = tree->_insFilePos;

			btreeWriteNode(node);
		}

		btreeSetRootNode(tree, node->block->offset);
		btreeDestroyNode(node);
	}
	
	free(newKey);

	return GDB_SUCCESS;
}

