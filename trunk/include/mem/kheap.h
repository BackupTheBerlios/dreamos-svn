/***************************************************************************
 *            kheap.h
 *
 *  Sat Mar 31 07:47:17 2007
 *  Copyright  2007  Ivan Gualandri
 *  Email
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef KHEAP_H
#define KHEAP_H

extern unsigned int address_cur;

typedef struct{
    unsigned int magic;
    unsigned int hole;
    unsigned int size;
} header_t;

typedef struct{
    unsigned int magic;
    unsigned int *header;
} footer_t;

void* kmalloc(unsigned int);
void kfree(unsigned int);

void *alloc(unsigned int);
// void free(unsigned int);


#endif