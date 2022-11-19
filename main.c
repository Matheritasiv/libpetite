#include<stddef.h>
#include"libpetite.h"

#define STR0(_X)  #_X
#define STR(_X)   STR0(_X)
#define BOOT      STR(__BOOT__)
extern const unsigned char boot_content[] __asm__ ("_binary_" BOOT "_start");
extern const unsigned char boot_end[]     __asm__ ("_binary_" BOOT "_end");
#ifdef __INIT__
extern void __INIT__(void);
#else
#define __INIT__ NULL
#endif

int main(int argc, const char *argv[])
{
	int ret;
	petite_init(boot_content, boot_end - boot_content, __INIT__);
	ret = Sscheme_start(argc, argv);
	petite_fini();
	return ret;
}
