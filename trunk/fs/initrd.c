/*
 * Dreamos
 * initrd.c
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stddef.h>
#include <initrd.h>
#include <multiboot.h>
#include <unistd.h>
#include <types.h>
#include <string.h>

extern char *module_start;
initrd_t *fs_specs;
initrd_file_t* fs_headers;

void dummy(){
	printf("Qui solo per una prova\n");
}	

int initfs_init(){	
	fs_specs = (initrd_t *) module_start;
	fs_headers = (initrd_file_t *)(module_start + sizeof(initrd_t));
	return fs_specs->nfiles;
	//printf("Number of files present: %d\n", fs_specs->nfiles);		
}

DIR *initfs_opendir(const char *path){
	printf("Sono una dummy_opendir, ti piaccio? path: %s\n", path);
	return NULL;
}

int initfs_open(const char *path, int flags, ...){
	initrd_file_t *module_var;
	int ifs_fd;
	ifs_fd = 0;
	int j = 0;
	module_var = fs_headers;	
	while (j < fs_specs->nfiles) {
		if(!strcmp(path, module_var[j].fileName)) printf("%s Found. Size: %d\n", path, module_var[j].length);
		j++;
	}
	return ifs_fd;
}

ssize_t initfs_read(int fildes, void *buf, size_t nbyte){
	printf("Hi i'm a dummy read... do you like me?\n");
	return nbyte;
}
