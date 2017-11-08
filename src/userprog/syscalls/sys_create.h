#define SCALL_CREATE_F(f) char *file_name;\
unsigned int size;\
if (!user_mem((f)->esp + SIZE, &file_name, sizeof(file_name)))\
	thread_exit();\
if (!user_mem((f)->esp + SIZE*2, &size, sizeof(file_name)))\
	thread_exit();\
(f)->eax = filesys_create(file_name, size);
