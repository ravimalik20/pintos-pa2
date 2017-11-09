#define SCALL_OPEN_F(f) sys_open_f((f))

static void put_in_list(struct fd_t *fd, struct list lst)
{
	int fd_id;

	if (list_empty(&lst)) {
		fd_id = 3;
	}
	else {
		struct list_elem *el = list_back(&lst);
		struct fd_t *fd_temp = list_entry(el, struct fd_t, elem);

		fd_id = fd_temp->id + 1;
	}

	fd->id = fd_id;
}

static void sys_open_f(struct intr_frame *f)
{
	char *file_name;

	if (!user_mem(f->esp + 4, &file_name, sizeof(file_name)))
		thread_exit();

	struct file *fl;
	struct fd_t *fd;

	fl = filesys_open(file_name);
	if (!fl)
		return false;

	fd->file = fl;

	struct thread *th = thread_current();
	struct list lst = th->fds;

	put_in_list(fd, lst);

	f->eax = fd->id;
}
