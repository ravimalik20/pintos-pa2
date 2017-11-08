#define SCALL_EXEC_F(f) void * input;\
if (!user_mem((f)->esp + SIZE, &input, sizeof(input)))\
	thread_exit();\
int child_id = process_execute(input);\
(f)->eax = (uint32_t) child_id;
