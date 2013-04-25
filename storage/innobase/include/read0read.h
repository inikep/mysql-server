/*****************************************************************************

Copyright (c) 1997, 2013, Oracle and/or its affiliates. All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA

*****************************************************************************/

/**************************************************//**
@file include/read0read.h
Cursor read

Created 2/16/1997 Heikki Tuuri
*******************************************************/

#ifndef read0read_h
#define read0read_h

#include "univ.i"


#include "ut0byte.h"
#include "ut0lst.h"
#include "trx0trx.h"
#include "read0types.h"

/*********************************************************************//**
Opens a read view where exactly the transactions serialized before this
point in time are seen in the view.
@return	own: read view struct */
UNIV_INTERN
read_view_t*
read_view_open_now(
/*===============*/
	trx_id_t	cr_trx_id,	/*!< in: trx_id of creating
					transaction, or 0 used in purge */
	mem_heap_t*	heap);		/*!< in: memory heap from which
					allocated */
/*********************************************************************//**
Makes a copy of the oldest existing read view, or opens a new. The view
must be closed with ..._close.
@return	own: read view struct */
UNIV_INTERN
read_view_t*
read_view_purge_open(
/*=================*/
	mem_heap_t*	heap);		/*!< in: memory heap from which
					allocated */
/*********************************************************************//**
Remove a read view from the trx_sys->view_list. */
UNIV_INLINE
void
read_view_remove(
/*=============*/
	read_view_t*	view,		/*!< in: read view, can be 0 */
	bool		own_mutex);	/*!< in: true if caller owns the
					trx_sys_t::mutex */
/*********************************************************************//**
Closes a consistent read view for MySQL. This function is called at an SQL
statement end if the trx isolation level is <= TRX_ISO_READ_COMMITTED. */
UNIV_INTERN
void
read_view_close_for_mysql(
/*======================*/
	trx_t*	trx);	/*!< in: trx which has a read view */
/*********************************************************************//**
Checks if a read view sees the specified transaction.
@return	true if sees */
UNIV_INLINE
bool
read_view_sees_trx_id(
/*==================*/
	const read_view_t*	view,	/*!< in: read view */
	trx_id_t		trx_id)	/*!< in: trx id */
	__attribute__((nonnull, warn_unused_result));

/** Read view lists the trx ids of those transactions for which a consistent
read should not see the modifications to the database. */

struct read_view_t{
	trx_id_t	low_limit_no;
				/*!< The view does not need to see the undo
				logs for transactions whose transaction number
				is strictly smaller (<) than this value: they
				can be removed in purge if not needed by other
				views */
	trx_id_t	low_limit_id;
				/*!< The read should not see any transaction
				with trx id >= this value. In other words,
				this is the "high water mark". */
	trx_id_t	up_limit_id;
				/*!< The read should see all trx ids which
				are strictly smaller (<) than this value.
				In other words, this is the "low water mark". */
	ulint		n_trx_ids;
				/*!< Number of cells in the trx_ids array */
	trx_id_t*	trx_ids;/*!< Additional trx ids which the read should
				not see: typically, these are the read-write
				active transactions at the time when the read
				is serialized, except the reading transaction
				itself; the trx ids in this array are in a
				descending order. These trx_ids should be
				between the "low" and "high" water marks,
				that is, up_limit_id and low_limit_id. */
	trx_id_t	creator_trx_id;
				/*!< trx id of creating transaction */
	UT_LIST_NODE_T(read_view_t) view_list;
				/*!< List of read views in trx_sys */
};

#ifndef UNIV_NONINL
#include "read0read.ic"
#endif

#endif
