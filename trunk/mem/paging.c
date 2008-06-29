/*
 * Dreamos
 * paging.h
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
 
 /*
  * Autore Ivan Gualandri
  * Prima versione: 29/10/2007
  * Contiene le definizioni di alcuni tipi di dato :D
  */

#include <paging.h>
#include <fismem.h>
#include <stdio.h>
#include <video.h>
#include <stddef.h>
// #define DEBUG 1
unsigned int *current_page_dir;
unsigned int *current_page_table;

void init_paging(){
    int i;
    printf("Abilito Paging: In lavorazione....");
    _kprintOK();    
    current_page_dir = create_pageDir();
    #ifdef DEBUG
    printf("Pd baseAddress: %d\n", (unsigned int) current_page_dir);
    #endif
    i=0;    
    while(i<PD_LIMIT){
        current_page_dir[i] = 0x00000000;
        i++;
    }
    set_pagedir_entry(1023, (unsigned int)current_page_dir, PD_PRESENT|SUPERVISOR, 0);
    current_page_table=create_pageTable();
    i=0;
    while(i<PT_LIMIT){
        current_page_table[i] = 0x00000000;
        i++;
    }
    set_pagedir_entry(0, (unsigned int)current_page_table, PD_PRESENT|SUPERVISOR|WRITE,0);    
    i=0;
    while(i<PT_LIMIT){
        set_pagetable_entry(i,i*0x1000,SUPERVISOR|PD_PRESENT|WRITE,0);
        #ifdef DEBUG
        if(i<10) printf("cpt: %d\n", current_page_table[i]);
        #endif
        i++;
    }        
    load_pdbr((unsigned int)current_page_dir);
}

/**
  * Crea una nuova page_dir
  * @author Ivan Gualandri
  * @version 1.0
  * @return page_dir address
  */
unsigned int* create_pageDir(){
//     Page_Dir prova;        
    return request_pages(8, NOT_ADD_LIST);       
}

/**
  * Crea una nuova page_table
  * @author Ivan Gualandri
  * @version 1.0
  * @return page_table address
  */
unsigned int* create_pageTable(){
    return request_pages(8, NOT_ADD_LIST);       
}
/**
  * Modifica gli attributi di una page_dir con la paginazione disabilitata
  * @author Ivan Gualandri
  * @version 1.0
  * @param pos l'elemento della pagedir che andremo a modificare
  * @param base l'indirizzo base della pagetable associata
  * @param opt1 le opzioni dei primi 7 bit
  * @param opt2 Di norma possono essere anche 0
  * @return none
  */
void set_pagedir_entry(int pos, unsigned int base, unsigned char opt1 , unsigned char opt2){
    current_page_dir[pos] = (base&0xFFFFF000)|opt1|opt2;
    #ifdef DEBUG
    printf("base: %d, basepde: %d\n", base, current_page_dir[pos]);
    #endif
}

/**
  * Modifica gli attributi di una page_dir con la paginazione abilitata
  * @author Ivan Gualandri
  * @version 1.0
  * @param pd_entry l'elemento della pagedir che andremo a modificare
  * @param base l'indirizzo base della pagetable associata
  * @param opt1 le opzioni dei primi 7 bit
  * @param opt2 Di norma possono essere anche 0
  * @return none
  */
void set_pagedir_entry_ric(int pd_entry, unsigned int base, unsigned char opt1, unsigned char opt2){
    unsigned int* mod_address;    
    mod_address = 0xFFFFF000 + (pd_entry*4);
    *mod_address = (base&0xFFFFF000)|opt1|opt2;    
    #ifdef DEBUG
    printf("value for entry n.: %d is: %d\n", pd_entry,*mod_address);
    #endif
}

/**
  * Modifica gli attributi di una page_table con la paginazione disabilitata
  * @author Ivan Gualandri
  * @version 1.0
  * @param pos l'elemento della pagetable che andremo a modificare
  * @param base l'indirizzo base della pagetable associata
  * @param opt1 le opzioni dei primi 7 bit
  * @param opt2 Di norma possono essere anche 0
  * @return none
  */
void set_pagetable_entry(int pos, unsigned int base, unsigned char opt1, unsigned char opt2){
    current_page_table[pos] = (base&0xFFFFF000)|opt1|opt2;
    #ifdef DEBUG
    if(pos<10) printf("pos: %d, base: %d, basepte: %d\n",pos, base, current_page_table[pos]);
    #endif
}

/**
  * Modifica gli attributi di una page_table con la paginazione abilitata
  * @author Ivan Gualandri
  * @version 1.0
  * @param pd_entry l'elemento della pagedir che che contiene la pagetable interessta
  * @param pt_entry l'elemento della pagetable che andremo a modificare
  * @param base l'indirizzo base della pagetable associata
  * @param opt1 le opzioni dei primi 7 bit
  * @param opt2 Di norma possono essere anche 0
  * @return none
  */
void set_pagetable_entry_ric(int pd_entry, int pt_entry ,unsigned int base, unsigned char opt1, unsigned char opt2){
    unsigned int *mod_address;    
    mod_address = (0XFFC00000|(pd_entry<<12))+(pt_entry*4);
    *mod_address = (base&0xFFFFF000)|opt1|opt2;
    #ifdef DEBUG
    printf("value for entry n.: %d is: %d\n", pt_entry,*mod_address);
    #endif
}

/**
  * Mostra il contenuto di una entry della page_dir
  * @author Ivan Gualandri
  * @version 1.0
  * @param num il numero della page_dir da leggere
  * @return Il contenuto dell'entry
  */
unsigned int get_pagedir_entry(int num){
    unsigned int *mod_address;
    mod_address= (0xFFFFF000 + (num*4));
    return *mod_address;
}

/**
  * Mostra il contenuto di una entry della page_dir
  * @author Ivan Gualandri
  * @version 1.0
  * @param dir_num il numero della page_dir da leggere
  * @param tab_num il numero della entry della pagetable che ci interessa
  * @return Il contenuto dell'entry
  */
unsigned int get_pagetable_entry(int dir_num, int tab_num){
    unsigned int *mod_address;
    mod_address=(0xFFC00000|(dir_num<<12))+ (tab_num*4);
    return (unsigned int) (*mod_address);
}

/**
  * Carica il registro pdbr con la nuova pagedir
  * @author Ivan Gualandri
  * @version 1.0
  * @param pdbase Indirizzo base della page_dir
  * @return none
  */
void load_pdbr(unsigned int pdbase){
    volatile unsigned int cr0, pdbr, tmp;
    cr0=0;
    tmp=0;
    asm ("movl %%cr0, %0":"=r" (cr0));    
    tmp=cr0|0x80000000;    
    pdbr = (pdbase&0xFFFFF000);    
    asm("movl %0, %%cr3\n"
         "movl %1, %%cr0\n"
        ::"r"(pdbr), "r" (tmp));
}
