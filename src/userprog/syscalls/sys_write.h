#ifndef __SYS_WRITE_H__
#define __SYS_WRITE_H__

extern struct lock lock_fs;

#define SCALL_WRITE_F(f) int fd;\
const void *buffer;\
unsigned size;\
\
if(user_mem((f)->esp + 4, &fd, 4) == false)\
	sys_exit(-1);\
if(user_mem((f)->esp + 8, &buffer, 4) == false)\
	sys_exit(-1);\
if(user_mem((f)->esp + 12, &size, 4) == false)\
	sys_exit(-1);\
int code = scall_write(fd, buffer, size);\
if(code <= 0)\
	sys_exit(-1);(f)->eax = (uint32_t) code;\

bool scall_write(int fd, const void *buffer, unsigned size)
{
	if (!(buffer < PHYS_BASE)) {
		thread_exit();

		return -1;
	}

	if (fd == 1) {
		lock_acquire(&lock_fs);

		putbuf (buffer, size);

		lock_release(&lock_fs);

		return size;
	}

	if (fd <= 0)
		return -1;

	struct fd_t *fd_obj = fdnum_to_fd(fd);
	if (!fd_obj) {
		return -1;
	}

	if (!fd_obj->file) {
		return -1;
	}

	lock_acquire(&lock_fs);

	file_write(fd_obj->file, buffer, size);

	lock_acquire(&lock_fs);

	return size;
}

#endif
