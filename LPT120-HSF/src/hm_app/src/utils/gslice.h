/* GLIB sliced memory - fast threaded memory chunk allocator
 * Copyright (C) 2005 Tim Janik
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __G_SLICE_H__
#define __G_SLICE_H__
#include "RCLplatform.h"

void *g_slice_alloc0(size_t size);

#define  g_slice_new(type)      ((type*) malloc (sizeof (type)))
#define  g_slice_new0(type)     ((type*) g_slice_alloc0 (sizeof (type)))
#define	 g_slice_free(type, mem)    \
									do { \
										free(mem); \
										mem=NULL; \
									} while(0)

#endif /* __G_SLICE_H__ */
