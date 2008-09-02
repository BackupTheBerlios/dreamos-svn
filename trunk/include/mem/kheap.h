/***************************************************************************
 *            kheap.h
 *
 *  Sun 18 07 08 07:47:17 2007
 *  Copyright  2008  Ivan Gualandri
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

#include <ordered_array.h>
extern unsigned int address_cur;

//Da eliminare
typedef struct{
    unsigned int magic;
    unsigned int hole;
    unsigned int size;
} header_t;

typedef struct{
    unsigned int magic;
    unsigned int *header;
} footer_t;
//Fino qua

/*!  \struct heap_node_t
     \brief Struttura dati che mantiene le informazioni su locazioni occupate e libere di memoria
 */
typedef struct {
    unsigned int start_address;/**< L'indirizzo di partenza dell'heap*/
    unsigned int size;    /**< quanto massimo si puo' espandere*/
    struct heap_node_t* next;
}heap_node_t;

/*!  \struct heap_t
     \brief Struttura dati che mantiene le informazioni su un singolo heap
 */
typedef struct{
    heap_node_t* free_list;/**< Locazioni di memoria libere*/
    heap_node_t* used_list;/**< Locazioni di memoria occupate*/
    unsigned int max_size; /**< Massima memoria allocabile */
} heap_t;


heap_t* make_heap(unsigned int, unsigned int, unsigned int); //Rivedere il return value
void* kmalloc(unsigned int);
void kfree(unsigned int);
void try_alloc();

void* alloc(unsigned int, heap_t*);

#endif