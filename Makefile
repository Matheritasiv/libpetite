TYPE	:= elf64
ENDIAN	:= 
MARCH	:= i386:
ARCH	:= x86-64

all: libpetite.so libpetite.a main.o

libpetite.so: scheme/*.o zlib/*.o lz4/*.o wrap.o boot.o
	gcc -s -Wl,-O2 -shared -lm -lncurses -luuid $^ -o$@

libpetite.a: scheme/*.o zlib/*.o lz4/*.o wrap.o boot.o
	-rm -f $@
	ar rc $@ $^
	ranlib $@

wrap.o: wrap.c
	gcc -c -fPIC -O2 $^ -o$@

boot.o: petite.boot
	objcopy -Ibinary -O$(TYPE)-$(ENDIAN)$(ARCH) -B$(MARCH)$(ARCH) $< $@

main.o: main.c
	gcc -c -fPIC -O2 -D__BOOT__=app_boot $^ -o$@

.PHONY: all
