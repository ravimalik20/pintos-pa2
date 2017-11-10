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
	else if (scall_num == SYS_READ) {
		int fd;
		void *buffer;
		unsigned size;
		uint32_t i;
		struct fd_t *fd_obj; 

		if(user_mem(f->esp + 4, &fd, 4))
			thread_exit();
		if(user_mem(f->esp + 8, &buffer, 4))
			thread_exit();
		if(user_mem(f->esp + 12, &size, 4))
			thread_exit();

		if (fd == 0) {
			for (i = 0 ; i < size ; i++) {
				((uint8_t *)buffer)[i] = input_getc();
			}

			f->eax = size;
		}
		else {
			fd_obj = fdnum_to_fd(fd);

			if (!fd_obj)
				f->eax = -1;
			else if (!fd_obj->file)
				f->eax = -1;
			else {
				f->eax = file_read(fd_obj->file, buffer, size);
			}
		}
	}
	else if (scall_num == SYS_EXEC) {
		SCALL_EXEC_F(f);
	}
	else if (scall_num == SYS_CREATE) {
		char *file_name;
		unsigned int size;

		if (!user_mem((f)->esp + SIZE, &file_name, sizeof(file_name)))
			thread_exit();
		if (!user_mem((f)->esp + SIZE*2, &size, sizeof(file_name)))
			thread_exit();

		lock_acquire(&lock_fs);

		f->eax = filesys_create(file_name, size);

		lock_release(&lock_fs);
	}
	else if (scall_num == SYS_REMOVE) {
		SCALL_REMOVE_F(f);
	}
	else if (scall_num == SYS_OPEN) {
		SCALL_OPEN_F(f);
	}
	else if (scall_num == SYS_FILESIZE) {
		int fd;
		struct fd_t *fd_obj;

		if (!user_mem(f->esp + 4, &fd, sizeof(&fd)))
			thread_exit();

		fd_obj = fdnum_to_fd(fd);

		if (!fd_obj) {
			f->eax = -1; thread_exit();
		}
		else if (!fd_obj->file) {
			f->eax = -1; thread_exit();
		}
		else {
			f->eax = file_length(fd_obj->file);
		}
	}
	else if (scall_num == SYS_CLOSE) {
		int fd_num = user_mem(f->esp + 4, &fd_num, sizeof(fd_num));

		if (fd_num == -1)
			thread_exit();

		check_user_address((uint8_t *) fd_num);

		struct fd_t *fd = fdnum_to_fd(fd_num);
		if (!fd) {
			f->eax = -1; thread_exit();
		}

		if (!fd->file) {
			f->eax = -1; thread_exit();
		}

		lock_acquire(&lock_fs);

		file_close(fd->file);

		lock_release(&lock_fs);

		list_remove(&(fd->elem));
		palloc_free_page(fd);
	}
	else if (scall_num == SYS_WAIT) {
		uint32_t pid;
		user_mem(f->esp + 4, &pid, sizeof(pid));

		f->eax = process_wait(pid);
	}

  //thread_exit ();
}

static void sys_exit(int code)
{
	printf("%s: exit(%d)\n", thread_current()->name, code);

	thread_exit ();
}
