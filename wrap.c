#include<windows.h>
#include"scheme.h"

#define petite_content _binary_petite_boot_start
#define petite_end     _binary_petite_boot_end
extern const unsigned char petite_content[];
extern const unsigned char petite_end[];
int init_state = 0;

EXPORT int petite_init(const unsigned char *boot_content, unsigned int boot_len, void (*custom_init)(void))
{
	if (init_state) return -1;
	init_state = 1;
	Sscheme_init(NULL);
	Sregister_boot_file_bytes("petite", (void *)petite_content, petite_end - petite_content);
	if (boot_content) {
		if (boot_len == 0) {
			Sregister_boot_file((const char *)boot_content);
		} else {
			Sregister_boot_file_bytes("app", (void *)boot_content, boot_len);
		}
	}
	Sbuild_heap(NULL, custom_init);
	return 0;
}

EXPORT int petite_fini(void)
{
	if (!init_state) return -1;
	Sscheme_deinit();
	init_state = 0;
	return 0;
}
