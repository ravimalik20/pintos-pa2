#define SCALL_REMOVE_F(f) char *file_name;\
unsigned int size;\
if (!user_mem((f)->esp + 4, &file_name, sizeof(file_name)))\
	thread_exit();\
(f)->eax = filesys_remove(file_name);
