VER     := 958

TYPE    := pe
ENDIAN  := 
MARCH   := i386:
ARCH    := x86-64

CL      := cl
LINK    := link
OBJCOPY := objcopy

CFLAGS  := /O2 /MD /Zi

all: petite.lib libpetite.dll main.obj

libpetite.dll: csv$(VER).lib scheme.res wrap_.obj boot.obj
	$(LINK) /dll /out:$@ /machine:X64 /nologo /wholearchive:$^ rpcrt4.lib ole32.lib advapi32.lib user32.lib

petite.lib: wrap_static.obj boot.obj
	$(LINK) /lib /out:$@ /nologo $^

wrap_.obj: wrap.c scheme.h
	-@del /q /f $@
	$(CL) /nologo /c $(CFLAGS) $<
	ren wrap.obj $@

wrap_static.obj: wrap.c scheme.h
	-@del /q /f $@
	$(CL) /nologo /c $(CFLAGS) /DSCHEME_STATIC $<
	ren wrap.obj $@

boot.obj: petite.boot
	$(OBJCOPY) -Ibinary -O$(TYPE)-$(ENDIAN)$(ARCH) -B$(MARCH)$(ARCH) $< $@

main.obj: main.c scheme.h
	$(CL) /nologo /c $(CFLAGS) /D__BOOT__=app_boot $<

.PHONY: all
