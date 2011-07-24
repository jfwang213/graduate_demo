# Project: baseline
# Created by Médéric Blestel <mblestel@insa-rennes.fr>

INCS = -I"../../../SVC/lib_svc" -I"../../../SVC/lib_utils" -I"." -I"../../h264_main_decoder/lib_main/"
CFLAGS = $(INCS) -DDISPLAY $(CXFLAGS) -DHAVE_SDL
SRC= $(wildcard *.c)

all: $(SRC:%.c=./obj/%.d) $(SRC%.c=%.o) libbaseline.a($(SRC:%.c=%.o))

./obj/%.d: %.c
	@mkdir -p obj
	@set -e; rm -f $@; \
	$(CC) $(INCS) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

-include $(SRC:.c=.d)

clean:
	rm -f *.o *.d *.a *.bak *.~
	rm -rf obj
 
