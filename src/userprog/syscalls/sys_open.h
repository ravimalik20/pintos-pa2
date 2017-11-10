extern struct lock lock_fs;

#define SCALL_OPEN_F(f) sys_open_f((f))

static void put_in_list(struct fd_t *fd, struct list *lst)
{
	int fd_id;

	if (list_empty(lst)) {
		fd_id = 3;
	}
	else {
		fd_id = (list_entry(list_back(lst), struct fd_t, elem)->id) + 1;
	}

	fd->id = fd_id;
}

static void sys_open_f(struct intr_frame *f)
{
	char *file_name;

	if (!user_mem(f->esp + 4, &file_name, sizeof(file_name))) {
		thread_exit();
		f->eax = -1;
	}

	struct file *fl;
	struct fd_t *fd = palloc_get_page(0);

	if (get_user((uint8_t *) file_name) == -1)
		thread_exit(); f->eax = -1;

	lock_acquire(&lock_fs);

	fl = filesys_open(file_name);

	lock_release(&lock_fs);

	if (!fl) {
		f->eax = -1;
	}
	else {
		fd->file = fl;

		struct list *lst = &thread_current()->fds;

		put_in_list(fd, lst);

		list_push_back (&lst, &(fd->elem));

		f->eax = fd->id;
	}
}
