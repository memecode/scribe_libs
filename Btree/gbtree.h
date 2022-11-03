#ifndef _GBTREE_H_
#define _GBTREE_H_

#include "btree.h"

class LBTree
{
	GDatabase *db;
	BTree *bt;

public:
	LBTree(const char *file = 0);
	~LBTree();

	bool Open(const char *file);
	bool IsOpen() { return db != 0 && bt != 0; }
	bool Close();

	bool Insert(const char *Key, long Value);
	bool Delete(const char *Key);
	bool Find(const char *Key, long &Result);
	unsigned long Length();

	void Print();
};

#endif