VER     := 958

TYPE    := pe
ENDIAN  := 
MARCH   := i386:
ARCH    := x86-64

CC      := cc
LINK    := link
OBJCOPY := objcopy

all: libpetite.dll petite.lib main.obj

libpetite.dll: csv$(VER).lib scheme.res wrap.obj boot.obj
	$(LINK) /dll /out:$@ /machine:X64 /nologo /wholearchive:$^ rpcrt4.lib ole32.lib advapi32.lib user32.lib

petite.lib: wrap_static.obj boot.obj
	$(LINK) /lib /out:$@ /nologo $^

wrap.obj: wrap.c scheme.h
	$(CC) -c -O2 $< -o$@

wrap_static.obj: wrap.c scheme.h
	$(CC) -c -O2 -DSCHEME_STATIC $< -o$@

boot.obj: petite.boot
	$(OBJCOPY) -Ibinary -O$(TYPE)-$(ENDIAN)$(ARCH) -B$(MARCH)$(ARCH) $< $@

main.obj: main.c scheme.h
	$(CC) -c -O2 -D__BOOT__=app_boot $< -o$@

.PHONY: all
