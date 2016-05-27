/*
 * driver/dma/dma.c
 *
 * Copyright(c) 2007-2016 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <xboot.h>
#include <dma/dma.h>

extern u8_t __dma_start[];
extern u8_t __dma_end[];
static void * __dma_pool = NULL;

void * dma_alloc(size_t size)
{
	return mm_memalign(__dma_pool, 4096, size);
}

void * dma_zalloc(size_t size)
{
	void * ptr = dma_alloc(size);

	if(ptr)
		memset(ptr, 0, size);
	return ptr;
}

void dma_free(void * ptr)
{
	mm_free(__dma_pool, ptr);
}

void do_init_dma_pool(void)
{
	__dma_pool = mm_create((void *)__dma_start, (size_t)(__dma_end - __dma_start));
}
