INCS = -I"../libview"
LIBS = -L"../bin/" -l viewsdl -lSDL
CFLAGS = $(INCS) $(CXFLAGS)
BIN = ../bin/YuvPlayer
all:	main.o
	$(CC) $< $(LIBS) -o $(BIN)
main.o:	main.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f main.o $(BIN)
