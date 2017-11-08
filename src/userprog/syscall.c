#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "userprog/process.h"
#include "devices/shutdown.h"
#include "helper.h"

#include "syscalls/sys_write.h"
#include "syscalls/sys_exec.h"

#define SUCCESS true
#define FAILURE false

#define ERR_MEM_INVALID -1

uint32_t sys_exec(const char *cmdline);

static void sys_exit(int code);
static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
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

		user_mem(f->esp + 4, &status, sizeof(status));

		sys_exit(status);
	}
	else if (scall_num == SYS_WRITE) {
		SCALL_WRITE_F(f);
	}
	else if (scall_num = SYS_EXEC) {
		SCALL_EXEC_F(f);
	}

  //thread_exit ();
}

static void sys_exit(int code)
{
	printf("%s: exit(%d)\n", thread_current()->name, code);

	thread_exit ();
}
