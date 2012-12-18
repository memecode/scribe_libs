/**
 * @file libpackman/types.h Type definitions.
 * 
 * $Id: types.h,v 1.7 2002/03/15 05:52:54 chipx86 Exp $
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
#ifndef _LIBPACKMAN_TYPES_H_
#define _LIBPACKMAN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The returned status of a function.
 */
typedef enum
{
	PM_SUCCESS,        /**< A successful completion.                     */
	PM_FAILED,         /**< A failed attempt.                            */
	PM_DB_ERROR,       /**< An error with a package database.            */
	PM_PACKAGE_ERROR,  /**< An error with a package.                     */
	PM_BATCH_ERROR,    /**< An error with a batch.                       */
	PM_NOT_SUPPORTED,  /**< The feature is not supported in that module. */
	PM_READ_ONLY,      /**< The package or database is read-only.        */
	PM_DEP_ERROR,      /**< Missing dependency, or other similar error.  */
	PM_EXISTS          /**< Package already exists in the database.      */

} PmStatus;

/**
 * The access mode used when opening a package or database.
 */
typedef enum
{
	/**
	 * Allows reading and writing to package files and database entries.
	 */
	PM_MODE_READ_WRITE,
	
	/**
	 * Allows reading, but not writing, of packages and database entries.
	 */
	PM_MODE_READ_ONLY,
	
	/*
	 * Enables test mode, where reading is allowed, but writing,
	 * file extraction, and database modification is disabled.
	 */
	PM_MODE_TEST,

	/*
	 * An error has occurred. Most likely, the specified package or
	 * database is NULL.
	 */
	PM_MODE_ERROR

} PmAccessMode;

/**
 * The relationship between two items (such as versions)
 */
typedef enum
{
	PM_REL_LESS_THAN = 0,    /**< Less than.                */
	PM_REL_LESS_EQUAL,       /**< Less than or equal to.    */
	PM_REL_EQUAL,            /**< Equal to.                 */
	PM_REL_GREATER_EQUAL,    /**< Greater than or equal to. */
	PM_REL_GREATER_THAN      /**< Greater than.             */

} PmRelationship;

#ifdef __cplusplus
}
#endif

#endif /* _LIBPACKMAN_TYPES_H_ */

