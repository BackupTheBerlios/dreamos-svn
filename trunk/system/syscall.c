/*
 * Dreamos
 * syscall.h
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
  * Autore Lisa Vitolo
  * Prima versione: 25/11/2007  
  */

#include <idt.h>
#include <handlers.h>
#include <syscall.h>
#include <stdio.h>
#include <video.h>

void (*syscall_table[SYSCALL_NUMBER])(int *) = {sysputch};

void sysputch(int *args)
{
   char s[2];
   s[0] = args[0];
   s[1] = '\0';

    _kputs (s);
}

/*
 * Gestore principale
 * DA CORREGGERE: Non riesco a pushare i registri perchè mi sballa i valori
 */
void syscall_handler()
{
     int eax=0;
     int ebx=0, ecx=0, edx=0;
     int arguments[3] = {0,0,0};

     asm ("movl %%eax, %0\n\t"
	  "movl %%ebx, %1\n\t"
	  "movl %%ecx, %2\n\t"
	  "movl %%edx, %3\n\t"
	  : "=r" (eax), "=r" (ebx), "=r" (ecx), "=r" (edx));

     arguments[0] = ebx;
     arguments[1] = ecx;
     arguments[2] = edx;
     
//      (*syscall_table[eax])(arguments);
    sysputch(arguments);
}

