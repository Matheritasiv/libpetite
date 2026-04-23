#include<stddef.h>
#include"scheme.h"

extern const unsigned char petite_content[] __asm__ ("_binary_petite_boot_start");
extern const unsigned char petite_end[]     __asm__ ("_binary_petite_boot_end");
int init_state = 0;

extern int petite_init(const unsigned char *boot_content, unsigned int boot_len, void (*custom_init)(void))
{
	if (init_state) return -1;
	init_state = 1;
	Sscheme_init(NULL);
	Sregister_boot_file_bytes("petite", (void *)petite_content, petite_end - petite_content);
	Sregister_boot_file_bytes("app", (void *)boot_content, boot_len);
	Sbuild_heap(NULL, custom_init);
	return 0;
}

extern int petite_fini(void)
{
	if (!init_state) return -1;
	Sscheme_deinit();
	init_state = 0;
	return 0;
}
