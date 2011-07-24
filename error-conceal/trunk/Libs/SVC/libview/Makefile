# Project: lib_view
# Makefile created by Dev-C++ 4.9.9.2


OBJ  = sdl.o extract_picture.o
LINKOBJ = sdl.o extract_picture.o
BIN  = ../bin/libviewsdl.a
INCS = -I"./include"

CFLAGS = $(INCS) -DDISPLAY $(CXFLAGS) -DHAVE_SDL -D_LOG_

RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(LINKOBJ)
	ar r $(BIN) $(LINKOBJ)
	ranlib $(BIN)

sdl.o: sdl.c
	$(CC) -c sdl.c -o sdl.o $(CFLAGS)

extract_picture.o: extract_picture.c
	$(CC) -c extract_picture.c -o extract_picture.o $(CFLAGS)
