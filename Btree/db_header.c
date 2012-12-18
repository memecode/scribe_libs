/**
 * @file db_header.h GNUpdate database header functions
 *
 * $Id: db_header.c,v 1.6 2002/04/07 18:29:41 chipx86 Exp $
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

char
gdbReadHeader(GDatabase *db)
{
	char version[2];
	char buffer[DB_HEADER_DATA_SIZE];
	int counter;
	
	if (db == NULL || db->fp == NULL)
		return 0;
	
	fseek(db->fp, 0, SEEK_SET);

	if (fread(buffer, DB_HEADER_DATA_SIZE, 1, db->fp) != 1)
	{
		fprintf(stderr, _("Error: Truncated database.\n"));

		return 0;
	}

	/* Check the magic string. */
	if (strncmp(buffer + DB_OFFSET_MAGIC, DB_MAGIC, 8))
	{
		fprintf(stderr, _("Error: Invalid database signature.\n"));

		return 0;
	}

	counter = 8;

	version[0] = gdbGet8(buffer, &counter);
	version[1] = gdbGet8(buffer, &counter);
	
	if (version[0] != DB_MAJOR_VER || version[1] != DB_MINOR_VER)
	{
		fprintf(stderr, _("Error: Unsupported database version %d.%d\n"),
				version[0], version[1]);

		return 0;
	}

	db->type = (GdbType) gdbGet8(buffer, &counter);

	if (db->type != GDB_INDEX_FILE && db->type != GDB_DATA_FILE)
	{
		fprintf(stderr, _("Error: Unsupported database type.\n"));

		return 0;
	}

	return 1;
}

void
gdbWriteHeader(GDatabase *db)
{
	char version[2];
	
	if (db == NULL || db->fp == NULL)
		return;

	version[0] = DB_MAJOR_VER;
	version[1] = DB_MINOR_VER;

	fseek(db->fp, 0, SEEK_SET);

	/* Write the magic string. */
	fputs(DB_MAGIC, db->fp);

	fwrite(version,             sizeof(char),     2, db->fp);
	fwrite(&db->type,           sizeof(char),     1, db->fp);

	if (DB_HEADER_BLOCK_SIZE > DB_HEADER_DATA_SIZE)
		gdbPad(db->fp, DB_HEADER_BLOCK_SIZE - DB_HEADER_DATA_SIZE);

	fflush(db->fp);
}

