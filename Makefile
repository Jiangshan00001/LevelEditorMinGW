CC=i686-w64-mingw32-gcc
WINDRES=i686-w64-mingw32-windres
RM=rm -rf
CCFLAGS = -nostdlib -nodefaultlibs -std=c99 -Wl,-e_start -s -mwindows -m32 -Os -w -march=i486
LIBS    = -luser32 -lkernel32 -lgdi32 -lwinmm -lcomctl32 -ladvapi32 -lshell32 -lmsvcrt

all: LevelEditor.exe
	

LevelEditor.res: LevelEditor.rc
	$(WINDRES) --input-format=rc -O coff -i LevelEditor.rc -o LevelEditor.res

LevelEditor.exe: main.c about.c selfile.c LevelEditor.res resource.h Icon.ico
	$(CC) $(CCFLAGS) main.c about.c selfile.c LevelEditor.res $(LIBS) -o $@

clean:
	$(RM) LevelEditor.res LevelEditor.exe .vs
