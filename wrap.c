#include<assert.h>
#include<fcntl.h>
#include<windows.h>
#include"scheme.h"

#define petite_content _binary_petite_boot_start
#define petite_end     _binary_petite_boot_end
extern const unsigned char petite_content[];
extern const unsigned char petite_end[];
int init_state = 0;

typedef struct targs {
	HANDLE pipe_handle;
	const unsigned char *content;
	unsigned int length;
} TARGS;

static DWORD WINAPI data_writer(LPVOID targs)
{
	HANDLE pipe_handle           = ((TARGS *)targs)->pipe_handle;
	const unsigned char *content = ((TARGS *)targs)->content;
	unsigned int length          = ((TARGS *)targs)->length;
	DWORD written;
	assert((WriteFile(pipe_handle, content, length, &written, NULL) != 0) &&\
		((unsigned int)written == length));
	CloseHandle(pipe_handle);
	return 0;
}

EXPORT int petite_init(const unsigned char *boot_content, unsigned int boot_len, void (*custom_init)(void))
{
	HANDLE thread_petite, thread_boot;
	if (init_state) return -1;
	init_state = 1;
	Sscheme_init(NULL);
	{
		HANDLE pipe_rd, pipe_wr;
		int fd_pipe;
		TARGS targs = {
			.content = petite_content,
			.length  = petite_end - petite_content
		};
		assert(CreatePipe(&pipe_rd, &pipe_wr, NULL, 0) != 0);
		targs.pipe_handle = pipe_wr;
		assert((thread_petite = CreateThread(\
			NULL, 0, data_writer, &targs, 0, NULL\
		)) != NULL);
		assert((fd_pipe = _open_osfhandle((intptr_t)pipe_rd, _O_RDONLY)) != -1);
		Sregister_boot_file_fd("petite", fd_pipe);
	}
	if (boot_content == NULL) {
		thread_boot = NULL;
	} else if (boot_len == 0) {
		thread_boot = NULL;
		Sregister_boot_file((const char *)boot_content);
	} else {
		HANDLE pipe_rd, pipe_wr;
		int fd_pipe;
		TARGS targs = {
			.content = boot_content,
			.length  = boot_len
		};
		assert(CreatePipe(&pipe_rd, &pipe_wr, NULL, 0) != 0);
		targs.pipe_handle = pipe_wr;
		assert((thread_boot = CreateThread(\
			NULL, 0, data_writer, &targs, 0, NULL\
		)) != NULL);
		assert((fd_pipe = _open_osfhandle((intptr_t)pipe_rd, _O_RDONLY)) != -1);
		Sregister_boot_file_fd("app", fd_pipe);
	}
	Sbuild_heap(NULL, custom_init);
	WaitForSingleObject(thread_petite, INFINITE);
	CloseHandle(thread_petite);
	if (thread_boot != NULL) {
		WaitForSingleObject(thread_boot, INFINITE);
		CloseHandle(thread_boot);
	}
	return 0;
}

EXPORT int petite_fini(void)
{
	if (!init_state) return -1;
	Sscheme_deinit();
	init_state = 0;
	return 0;
}
