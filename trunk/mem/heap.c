/***************************************************************************
 *            heap.c
 *
 *  Sun 8 08 10 
 *  Copyright  2010  Ivan Gualandri
 *  Email finarfin@elenhost.org
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <heap.h>
#include <stdio.h>
#include <ordered_array.h>
#include <kheap.h>

extern unsigned int end;
extern unsigned int address_cur;
new_heap_t *n_heap;


void init_newmem(){
	n_heap =0;
	n_heap = (new_heap_t*)new_heap(HEAP_START_ADDRESS, HEAP_INDEX_SIZE);	
}

short int header_t_less_than(void *a,void *b){
	return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

new_heap_t *new_heap(unsigned int start, unsigned int size){
	/*Actually we need to use kmalloc!*/
	new_heap_t* t_heap =(new_heap_t*) kmalloc(sizeof(new_heap_t));		
	if(start%0x1000 == 0){		
		header_t *first_hole = (header_t*)start;		
		t_heap->index = new_array((void*)start,size, &header_t_less_than);
		t_heap->start_address = start;
		t_heap->end_address = start + size;					
		first_hole->size = t_heap->end_address - start;		
		first_hole->magic = HEAP_MAGIC;		
		first_hole->is_hole = 1; /*TRUE*/			
		insert_array((void*) first_hole, &t_heap->index);
		//printf("First hole: %u size: %u", t_heap->start_address, t_heap->end_address);
		//get_array(0, &t_heap->index);
		//locate_smallest_hole(size, PAGE_ALIGNED,&t_heap->index);
		return t_heap;
	}
	else printf("ERROR\n");
	return 0;
}

unsigned int new_malloc(unsigned int size){
	if(n_heap==0) {
		//I have no heap defined, i continue to use address_cur.
		unsigned int tmp;
		printf("No heap defined starting %x...\n", address_cur);
		tmp = address_cur; 
		address_cur += size;
		printf("New address: %x\n", address_cur);
		return tmp;
	}
	else {
		//If i have a heap, then i have vm management enabled, and i can call alloc...
		void* new_address = new_alloc(size, PAGE_ALIGNED, n_heap);	
		printf("Heap defined\n");
		printf("Start Address: %u End Address: %u\n", n_heap->start_address, n_heap->end_address);
		printf("See address_cur value: %x\n", address_cur);
		return 0;
	}
	return 0;
}

void *new_alloc(unsigned int size, unsigned short int p_aligned, new_heap_t* t_heap){
	//#define MEMDEBUG
	unsigned int real_size = size +sizeof(header_t) + sizeof(footer_t);	
	printf("Size of:\n\theader_t: %d\n\tfooter_t: %d\n\treal_size: %d\n\tsize: %d\n", sizeof(header_t), sizeof(footer_t), real_size, size);	
	#ifdef  MEMDEBUG
	unsigned int min_index = locate_smallest_hole(real_size, PAGE_ALIGNED, t_heap);
	if(min_index == -1 ){
		/*No hole with the requested size found, asking more space for heap*/
		printf("No hole  found we need to grow\n");
	}
	else {
		printf("Good News: Hole found\n");
		//get_array(0, &t_heap->index);
	}	
	#endif
}

short int locate_smallest_hole(unsigned int size, unsigned short int p_align, new_heap_t* in_heap){
	unsigned int index = 0; 
	printf("Size index: %d\n", in_heap->index.size);
	while(index < in_heap->index.size){
		unsigned int h_size;
		header_t *header = (header_t *)get_array(index, &in_heap->index);
		printf("Header: 0x%x\tSize: 0x%x\n", header->magic, header->size);
		h_size = header->size;
		if(h_size >= size)
			break;
		index++;
	}
	if(index >= in_heap->index.size)
		return -1;
	else 
		return index;
}
