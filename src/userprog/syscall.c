#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "helper.h"
#include "threads/synch.h"
#include "lib/kernel/list.h"

#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/palloc.h"

#include "syscalls/sys_write.h"
#include "syscalls/sys_exec.h"
#include "syscalls/sys_create.h"
#include "syscalls/sys_remove.h"
#include "syscalls/sys_open.h"

#define SUCCESS true
#define FAILURE false

#define ERR_MEM_INVALID -1

#define SIZE 4

uint32_t sys_exec(const char *cmdline);

static void sys_exit(int code);
static void syscall_handler (struct intr_frame *);

int sys_open(const char* file);

struct lock lock_fs;

void
syscall_init (void) 
{
	lock_init (&lock_fs);

  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
	/* Reading data from user: syscall number */
	int scall_num = * (int *)f->esp;

	#ifdef debug
	printf("__DEBUG__ : Syscall number is: %d\n", scall_num);
	#endif

	/* Process system call */

	if (scall_num == SYS_HALT) {
		shutdown_power_off();
	}
	else if (scall_num == SYS_EXIT) {
		int status = 0;

		user_mem(f->esp + SIZE, &status, sizeof(status));

		sys_exit(status);
	}
	else if (scall_num == SYS_WRITE) {
		SCALL_WRITE_F(f);
	}
	else if (scall_num == SYS_EXEC) {
		SCALL_EXEC_F(f);
	}
	else if (scall_num == SYS_CREATE) {
		SCALL_CREATE_F(f);
	}
	else if (scall_num == SYS_REMOVE) {
		SCALL_REMOVE_F(f);
	}
	else if (scall_num == SYS_OPEN) {
		SCALL_OPEN_F(f);
	}
	else if (scall_num == SYS_CLOSE) {
		int fd_num = user_mem(f->esp + 4, &fd_num, sizeof(fd_num));

		if (fd_num == -1)
			thread_exit();

		//check_user_address((uint8_t *) fd_num);

		struct fd_t *fd = fdnum_to_fd(fd_num);
		if (!fd)
			thread_exit();

		if (!fd->file)
			thread_exit();

		file_close(fd->file);
		list_remove(&(fd->elem));
		palloc_free_page(fd);
	}

  //thread_exit ();
}

static void sys_exit(int code)
{
	printf("%s: exit(%d)\n", thread_current()->name, code);

	thread_exit ();
}
