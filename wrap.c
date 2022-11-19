#include<assert.h>
#include<fcntl.h>
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include"scheme.h"

extern const unsigned char petite_content[] __asm__ ("_binary_petite_boot_start");
extern const unsigned char petite_end[]     __asm__ ("_binary_petite_boot_end");
int init_state = 0;

typedef struct targs {
	int pipe_des;
	const unsigned char *content;
	unsigned int length;
} TARGS;

static void *data_writer(void *targs)
{
	int pipe_des                 = ((TARGS *)targs)->pipe_des;
	const unsigned char *content = ((TARGS *)targs)->content;
	unsigned int length          = ((TARGS *)targs)->length;
	assert(write(pipe_des, content, length) == length);
	close(pipe_des);
	return NULL;
}

extern int petite_init(const unsigned char *boot_content, unsigned int boot_len, void (*custom_init)(void))
{
	pthread_t pid_petite, pid_boot;
	if (init_state) return -1;
	init_state = 1;
	Sscheme_init(NULL);
	{
		int pipe_des[2];
		TARGS targs = {
			.content = petite_content,
			.length  = petite_end - petite_content
		};
		assert(pipe(pipe_des) == 0);
		targs.pipe_des = pipe_des[1];
		assert(pthread_create(&pid_petite, NULL, data_writer, &targs) == 0);
		Sregister_boot_file_fd("petite", pipe_des[0]);
	}
	{
		int pipe_des[2];
		TARGS targs = {
			.content = boot_content,
			.length  = boot_len
		};
		assert(pipe(pipe_des) == 0);
		targs.pipe_des = pipe_des[1];
		assert(pthread_create(&pid_boot, NULL, data_writer, &targs) == 0);
		Sregister_boot_file_fd("app", pipe_des[0]);
	}
	Sbuild_heap(NULL, custom_init);
	pthread_join(pid_petite, NULL);
	pthread_join(pid_boot, NULL);
	return 0;
}

extern int petite_fini(void)
{
	if (!init_state) return -1;
	Sscheme_deinit();
	init_state = 0;
	return 0;
}
