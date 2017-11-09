#ifndef __HELPER_H__
#define __HELPER_H__

bool user_mem (void *source, void *destination, int bytes)
{
	int val = 0, i;

	for (i = 0; i < bytes; i++) {
		void *address = (void *)(source + i);

		if (!(address < PHYS_BASE))
			return false;

		val = get_user(address);

		if(val < 0)
			return false;

		*(char *)(destination + i) = val & 0xff;
	}

	return true;
}

/* Taken from pintos manual */
int get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

bool check_user_address(uint8_t *add)
{
	if (get_user((uint8_t*) add) == -1)
		thread_exit();
}

struct fd_t *fdnum_to_fd (int num)
{
	struct thread *th = thread_current();

	struct list fds = th->fds;

	for (struct list_elem* iter = list_begin(&fds); iter != list_end(&fds);
	iter = list_next(iter)) {
		struct fd_t *fd = list_entry(iter, struct fd_t, elem);

		if (fd->id == num)
			return fd;
		else
			return NULL;
	}
}

#endif
