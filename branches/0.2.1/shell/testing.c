#include <commands.h>
#include <multiboot.h>
#include <kernel.h>
#include <stddef.h>
#include <video.h>
#include <pic8259.h>
#include <8253.h>
#include <gdt.h>
#include <idt.h>
#include <cpuid.h>
#include <stdio.h>
#include <string.h>
#include <fismem.h>
#include <io.h>
#include <keyboard.h>
#include <paging.h>
#include <use.h>
#include <shell.h>
#include <version.h>
#include <cpuid.h>
#include <clock.h>
#include <sys/utsname.h>
#include <mouse.h>
#include <vfs.h>
#include <testing.h>
#include <multiboot.h>
#include <kernel.h>
#include <fcntl.h>
#include <initrd.h>
#include <unistd.h>
#include <user_shell.h>

//multiboot_info_t *boot_informations;
char *module_start;
file_descriptor_t fd_list[_SC_OPEN_MAX];

void try_strtok()
{
	char *s = "Hello World";
	char *p;
	
	printf("Stringa completa: %s\n"
		 "Stringa spezzata: \n" ,s);
	
	p = strtok(s, " ");
	while (p != NULL) {
		printf("%s\n", p);
		p = strtok(NULL, " ");
	}
}

void try_kmalloc()
{
	int *b, *c, *d;
	int i = 0;

	printf("Kmalloc try: ... ");
	//print_heap_list (kheap->free_list);
	b = (int *)kmalloc(15 * sizeof(int));
	c = (int *)kmalloc(10 * sizeof(int));
	d = (int *)kmalloc(15 * sizeof(int));
	printf("Address obtained: %d %d %d\n", b, c, d);
  
	while(i < 15) {
		b[i] = i*2;
		if(i < 10) c[i] = i*3;
		d[i] = i*2;
		i++;
	}
	i = 0;

	while(i < 15) {
		printf("b[%d] = %d d[%d] = %d ",i, b[i], i,d[i]);
		if(i < 10) printf("c[%d] = %d\n", i, c[i]);
		else printf("\n");
		i++;
	}
	
	printf("Navigating used list...\n");
	print_heap_list (kheap->used_list);
	free (b);
	free (c);
	free (d);
}

void do_fault()
{
	int *prova;

	printf ("Genero un pagefault scrivendo 10 nella locazione 0xa0000000...\n");
	prova = (int *)0xa0000000;
	*prova = 10;
	printf ("Contenuto della locazione 0xa0000000 dopo l'intervento dell'handler: %d\n", *prova);
}

void try_printmem(void)
{
	print_heap_list(kheap->used_list);
}

void help_tester()
{
	printf("Testing functions.. ");
	_kcolor(4);
	printf("Warning: for developers only!\n");
	_kcolor(7);
	printf(
		"  -> try_kmalloc       - Test a basic kmalloc() function\n"
		"  -> do_fault          - Test a page_fault\n"
		"  -> try_strtok        - Test strtok() function in string.h\n"
		"  -> try_printmem      - Print used locations of memory\n"		
		"  -> try_open          - Function to test open() & stdarg() \n"		
		"  -> try_syscall	    - Try some syscall functions\n"
		"  -> try_check         - Test username if exist\n"
		"  -> test_stat			- Test stat function\n"
		);	
}

void try_module(){
	initrd_t *fs_head;
	initrd_file_t* headers;
	int i;
	i=0;
	fs_head = (initrd_t *)module_start;
	headers = (initrd_file_t *) (module_start + sizeof(initrd_t));
	while(i<fs_head->nfiles){
		printf("%s\t%d\n", headers[i].fileName, headers[i].length);
		i++;
	}
	printf("Total Files: %d\n", fs_head->nfiles);
	_kputs("\n");
}

void try_check(){
	char test_name[50];
	memset(test_name, '\0', 50);
	printf("Please insert a username: "); 
	scanf("%s", test_name);
	user_chk(test_name, test_name);
}

void try_open(){
	char appoggio[50];
	char prova;
	int i;
	i=0;	
	while(i<50){
		appoggio[i] = '\0';
		i++;
	}
	printf("Please insert a path: ");
	
	scanf("%s", appoggio);	
	i = open(appoggio, O_RDONLY, 42);
	printf("%d\n", i);
	if(i>-1) {
		int j=0;		
		while(read(i, &prova, 1)!=0) {
			putchar(prova);			
			j++;
		}
		//printf("\n%s\n", prova);
		close(i);
	}
	printf("\n");
}

void try_ocreat(){
	int fd=0;
	//printf("Number of files present: %d\n", initfs_init());
	fd = open("pippo", O_RDWR|O_CREAT|O_APPEND);	
	if(fd>=0) write(fd, "buffo buffer", strlen("buffo buffer"));
	else printf("Error?\n");
	close(fd);	
}

void try_syscall(){
	int i;
	int var = -100;
	printf("Trying sysputch:\n");	
    for(i='A';i<='Z';i++) {
		asm(
			"movl %0, %%ecx\n"
			"movl $0x0, %%eax\n"
			"int $80\n"
			: : "g"(i)	
		);	
	}
	printf("And before ending try to print a -100: %d\n", var);
	printf("\n");
}

void show_fd(){
	int i=0;
	while(i < _SC_OPEN_MAX){
		if(fd_list[i].mountpoint_id == -1) printf("-1\n");
		else printf("+%d\n", fd_list[i].mountpoint_id);
		i++;
	}
	printf("\n");
}

void test_stat(){
	struct stat *stats;
	printf("Testing stat functions on README\n");
	stats = (struct stat*) kmalloc(sizeof(struct stat));
	stat("README", stats);
	printf("Device_id: %d\n", stats->st_dev);	
	printf("Size: %d\n", stats->st_size);
	printf("Uid: %d\n", stats->st_uid);
	free(stats);	
}

void try_shadow(){
	char stringa[30];
	memset(stringa, '\0', 30);
	set_shadow(ENABLED);
	scanf("%s", stringa);
	set_shadow(DISABLED);
	printf("%s\n", stringa);
}
