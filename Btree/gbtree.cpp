#include "gbtree.h"
#include "db_header.h"
#include "db_blocklist.h"

LBTree::LBTree(const char *file)
{
	db = 0;
	bt = 0;
	if (file)
		Open(file);
}

LBTree::~LBTree()
{
	Close();
}

bool LBTree::Open(const char *file)
{
	Close();

	db = gdbOpen(file, GDB_DATA_FILE, PM_MODE_READ_WRITE);
	if (db)
	{
		bt = btreeOpen(db, DB_MAIN_TREE_OFFSET);
		return bt != 0;
	}

	return false;
}

bool LBTree::Close()
{
	if (bt)
	{
		btreeClose(bt);
		bt = 0;
	}
	if (db)
	{
		gdbClose(db);
		db = 0;
	}

	return true;
}

bool LBTree::Insert(const char *Key, long Value)
{
	if (bt)
	{
		return btreeInsert(bt, Key, Value) != GDB_ERROR;
	}

	return false;
}

bool LBTree::Delete(const char *Key)
{
	if (bt)
	{
		return btreeDelete(bt, Key) != 0;
	}

	return false;
}

bool LBTree::Find(const char *Key, long &Result)
{
	if (bt)
	{
		Result = btreeSearch(bt, Key);
		return Result != 0;
	}

	return false;
}

unsigned long LBTree::Length()
{
	return bt ? btreeGetSize(bt) : 0;
}

void LBTree::Print()
{
	if (bt)
	{
		btreePrettyPrint(bt, bt->root, 0);
	}
}