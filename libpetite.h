#ifndef __LIBPETITE_H
#define __LIBPETITE_H

#include<stddef.h>
#include"scheme.h"

extern int petite_init(const unsigned char *, unsigned int, void (*)(void));
extern int petite_fini(void);

#endif
