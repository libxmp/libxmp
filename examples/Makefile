
CC ?= gcc
CFLAGS := -O3 -Wall -I../include $(CFLAGS)
LD = $(CC)
LDFLAGS ?=
LIBS = -lxmp

EXAMPLE_EXES	= player-simple player-showpatterns showinfo player-getbuffer player-openal player-openal-buffer
EXAMPLE_EXES_SDL= player-sdl player-sdl2 player-sdl-smix player-sdl2-smix

all: examples

examples: $(EXAMPLE_EXES)

player-simple: player-simple.o alsa.o
	$(LD) -o $@ $(LDFLAGS) $+ -lasound $(LIBS)

player-showpatterns: player-showpatterns.o alsa.o
	$(LD) -o $@ $(LDFLAGS) $+ -lasound $(LIBS)

showinfo: showinfo.o
	$(LD) -o $@ $(LDFLAGS) $+ $(LIBS)

player-getbuffer: player-getbuffer.o alsa.o
	$(LD) -o $@ $(LDFLAGS) $+ -lasound $(LIBS)

player-openal: player-openal.o
	$(LD) -o $@ $(LDFLAGS) $+ -lopenal $(LIBS)

player-openal-buffer: player-openal-buffer.o
	$(LD) -o $@ $(LDFLAGS) $+ -lopenal $(LIBS)


player-sdl: player-sdl.o
	$(LD) -o $@ $(LDFLAGS) $+ $$(pkg-config --libs sdl) $(LIBS)

player-sdl.o: player-sdl.c
	$(CC) $(CFLAGS) -c $+ -o $@ $$(pkg-config --cflags sdl)


player-sdl2: player-sdl2.o
	$(LD) -o $@ $(LDFLAGS) $+ $$(pkg-config --libs sdl2) $(LIBS)

player-sdl2.o: player-sdl.c
	$(CC) $(CFLAGS) -c $+ -o $@ $$(pkg-config --cflags sdl2)


player-sdl-smix: player-sdl-smix.o
	$(LD) -o $@ $(LDFLAGS) $+ $$(pkg-config --libs sdl) $(LIBS)

player-sdl-smix.o: player-sdl-smix.c
	$(CC) $(CFLAGS) -c $+ -o $@ $$(pkg-config --cflags sdl)


player-sdl2-smix: player-sdl2-smix.o
	$(LD) -o $@ $(LDFLAGS) $+ $$(pkg-config --libs sdl2) $(LIBS)

player-sdl2-smix.o: player-sdl-smix.c
	$(CC) $(CFLAGS) -c $+ -o $@ $$(pkg-config --cflags sdl2)

clean:
	@rm -f $(EXAMPLE_EXES)
	@rm -f $(EXAMPLE_EXES:=.exe)
	@rm -f $(EXAMPLE_EXES_SDL)
	@rm -f $(EXAMPLE_EXES_SDL:=.exe)
	@rm *.o
