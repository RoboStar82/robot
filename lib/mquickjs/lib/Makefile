
ifeq ($(OS),Windows_NT)
	EXE = .exe
	RM = del /q /f
else
	CFLAGS = -g -Wall -Os -D_GNU_SOURCE -I../src -fno-math-errno -fno-trapping-math
	LDFLAGS = -g
	EXE =
	RM = rm -f
endif

all: ../include/mquickjs_atom.h ../../../include/script_begin.h

../include/mquickjs_atom.h: script_stdlib$(EXE)
	./script_stdlib$(EXE) -a -m32 > $@

../../../include/script_begin.h: script_stdlib$(EXE)
	./script_stdlib$(EXE) -m32 > $@

script_stdlib$(EXE): script_stdlib.o mquickjs_build.o
	$(CC) $(LDFLAGS) -o $@ $^

script_stdlib.o: script_stdlib.c ../src/mquickjs_build.h
	$(CC) $(CFLAGS) -c -o $@ $<

mquickjs_build.o: ../src/mquickjs_build.c ../src/mquickjs_build.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	$(RM) script_stdlib$(EXE) script_stdlib.o mquickjs_build.o
