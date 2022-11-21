#include<stddef.h>
#include"libpetite.h"

#define BOOT_CONCAT(_X, _Y) _binary_##_X##_Y
#define boot_concat(_X, _Y) BOOT_CONCAT(_X, _Y)
#define boot_content        boot_concat(__BOOT__, _start)
#define boot_end            boot_concat(__BOOT__, _end)
extern const unsigned char boot_content[];
extern const unsigned char boot_end[];
#ifdef __INIT__
extern void __INIT__(void);
#else
#define __INIT__ NULL
#endif

int main(int argc, const char *argv[])
{
	petite_init(boot_content, boot_end - boot_content, __INIT__);
	int ret = Sscheme_start(argc, argv);
	petite_fini();
	return ret;
}
