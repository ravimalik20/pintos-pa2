#ifndef __SYS_WRITE_H__
#define __SYS_WRITE_H__

extern struct lock lock_fs;

#define SCALL_WRITE_F(f) int fd, return_code;\
const void *buffer;\
unsigned size;\
\
if(user_mem((f)->esp + 4, &fd, 4) == false)\
	sys_exit(-1);\
if(user_mem((f)->esp + 8, &buffer, 4) == false)\
	sys_exit(-1);\
if(user_mem((f)->esp + 12, &size, 4) == false)\
	sys_exit(-1);\
if(!scall_write(fd, buffer, size, &return_code))\
	thread_exit();(f)->eax = (uint32_t) return_code;\

bool scall_write(int fd, const void *buffer, unsigned size, int* ret)
{
	if (!(buffer < PHYS_BASE)) {
		thread_exit();

		return false;
	}

	if (fd == 1) {
		lock_acquire(&lock_fs);

		putbuf (buffer, size);

		lock_release(&lock_fs);

		*ret = size;

		return true;
	}

	return true;
}

#endif
