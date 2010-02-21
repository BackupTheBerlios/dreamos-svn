/*
 * Dreamos
 * unistd.c
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

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <vfs.h>

struct mountpoint_t mountpoint_list[MAX_MOUNTPOINT];
file_descriptor_t fd_list[_SC_OPEN_MAX];

ssize_t read(int fildes, void *buf, size_t nbyte){
	int mp_id = 0;
	int fs_fd;
	mp_id = fd_list[fildes].mountpoint_id;
	fs_fd = fd_list[fildes].fs_spec_id;
	//printf("Mpoint id: %d Fs_fd: %d\n", mp_id, fs_fd);
	if (mountpoint_list[mp_id].operations.read != NULL)
		return mountpoint_list[mp_id].operations.read(fs_fd,(char *) buf, nbyte);
	else
		printf("No READ Found for that file system\n");
	return 0;
}

int close(int fildes){
	int mp_id;
	int fs_fd;
	mp_id = fd_list[fildes].mountpoint_id;
	fs_fd = fd_list[fildes].fs_spec_id;
	if (fd_list[fildes].fs_spec_id >= 0){
		if(mountpoint_list[mp_id].operations.close !=NULL)
			mountpoint_list[mp_id].operations.close(fs_fd);		
		fd_list[fildes].fs_spec_id = -1;
		fd_list[fildes].mountpoint_id = -1;
		return 0;
	}
	else
		return -1;

return 0;
}
