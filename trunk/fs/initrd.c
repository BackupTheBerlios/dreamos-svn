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
#include <initrd.h>
#include <multiboot.h>
#include <unistd.h>
#include <types.h>
#include <string.h>
#include <vfs.h>
#include <kheap.h>

char *module_start;
initrd_t *fs_specs;
initrd_file_t* fs_headers;
initrd_fd ird_descriptors[MAX_INITRD_DESCRIPTORS];
unsigned int cur_irdfd;

void dummy(){
	printf("Qui solo per una prova\n");
}	

int initfs_init(){	
	int i=0;
	fs_specs = (initrd_t *) module_start;
	fs_headers = (initrd_file_t *)(module_start + sizeof(initrd_t));
	while (i<MAX_INITRD_DESCRIPTORS) {
		ird_descriptors[i].file_descriptor = -1;
		ird_descriptors[i].cur_pos = 0;
		i++;
	}
	cur_irdfd = 0;
	return fs_specs->nfiles;
	//printf("Number of files present: %d\n", fs_specs->nfiles);		
}

DIR *initfs_opendir(const char *path){
	DIR* pdir = NULL;
	initrd_file_t *module_var;
	module_var = fs_headers;
	if(!strcmp(path, "/") || path[0]== '\0'){
		//printf("Directory found\n");
		pdir = kmalloc(sizeof(DIR));
		strcpy(pdir->path, path);
		pdir->handle = 0x01;
		pdir->cur_entry = 0x00;
		pdir->entry.d_ino = 0x00;
		strcpy(pdir->entry.d_name, module_var[0].fileName);
		//printf("First occorrency: %s\n", pdir->entry.d_name);
		return pdir;
	}
	else {
		printf("Sono una dummy_opendir, ti piaccio? path: %s\n", path);	
		return NULL;
	}
}

struct dirent *initrd_readdir(DIR *dirp){
	initrd_file_t *fs_type;	
	int nfiles;	
	nfiles = fs_specs->nfiles;
	//printf("%d nfiles\n", nfiles);
	if(dirp->cur_entry < nfiles){	
		fs_type = (initrd_file_t *)(module_start + sizeof(initrd_t));	
		dirp->entry.d_ino =  dirp->cur_entry;
		dirp->entry.d_type = fs_type[dirp->cur_entry].file_type;
		strcpy(dirp->entry.d_name, fs_type[dirp->cur_entry].fileName);	
		//printf("%s\n", dirp->entry.d_name);
		dirp->cur_entry++;
		//printf("Placeholder for future readdir of initrd Number of files: %d\n", dirp->cur_entry);
		return &(dirp->entry);
	}
	else return NULL;
	return NULL;
}

int initfs_open(const char *path, int flags, ...){
	initrd_file_t *module_var;
	int j = 0;
	int ret_fd = -1;
	module_var = fs_headers;
	if(cur_irdfd >= MAX_INITRD_DESCRIPTORS) {
		int i=0;
		cur_irdfd=0;
		while(ird_descriptors[i].file_descriptor!=-1 && i < MAX_INITRD_DESCRIPTORS) i++;			
		cur_irdfd = i;
	}
	while (j < fs_specs->nfiles) {
		if(!strcmp(path, module_var[j].fileName)){
				if(module_var[j].file_type == FS_DIRECTORY || module_var[j].file_type == FS_MOUNTPOINT)
					return -1;
				ird_descriptors[cur_irdfd].file_descriptor	= j;
				//printf("%s Found. Size: %d FS fd val: %d - ID File val: %d\n", path, module_var[j].length, cur_irdfd, ird_descriptors[cur_irdfd].file_descriptor);
				ret_fd = cur_irdfd;				
				//printf("ret_fd: %d --- %d\n", cur_irdfd, j);
				ird_descriptors[cur_irdfd].cur_pos = 0;
				return cur_irdfd++; 
		}
		j++;
	}
	return -1;	
}

ssize_t initfs_read(int fildes, char *buf, size_t nbyte){
	char *file_start;
	int lfd, file_size;	
	int j=0;	
	int read_pos = 0;
	lfd = ird_descriptors[fildes].file_descriptor;
	read_pos = ird_descriptors[fildes].cur_pos;
	file_size = fs_headers[lfd].length;
	file_start = (char *) (module_start	+ fs_headers[lfd].offset);
	//printf("cur_pos val: %d\n", ird_descriptors[fildes].cur_pos);
	if(nbyte == 0) return 0;	
	while(j<nbyte && read_pos<file_size){		
		*buf++ = file_start[read_pos];			
		read_pos++;
		j++;
	}			
	ird_descriptors[fildes].cur_pos = read_pos;		
	if(read_pos == file_size) return 0;
	return nbyte;
}

int initrd_close(int fildes){
	ird_descriptors[fildes].file_descriptor = -1;
	ird_descriptors[fildes].cur_pos = 0;
	return 0;
}
