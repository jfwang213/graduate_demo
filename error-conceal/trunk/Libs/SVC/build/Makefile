# Project: SVC
# Modified by Mederic Blestel <mblestel@insa-rennes.fr>

DEBUG = no

OBJ = obj/PC.o obj/read_video.o obj/decoder_svc_layer_nal.o obj/decoder_base_layer_nal.o obj/cal_time.o obj/write_yuv_file.o
LINKOBJ  = obj/PC.o obj/read_video.o obj/decoder_svc_layer_nal.o obj/decoder_base_layer_nal.o obj/cal_time.o obj/write_yuv_file.o
INCS = -I"../../AVC/h264_baseline_decoder/lib_baseline" -I"../../h264_baseline_decoder/lib_baseline" -I"../../AVC/h264_main_decoder/lib_main" -I"../lib_svc" -I"/usr/include/SDL" -I"../lib_utils" -I"../libview"
BIN  = ../bin/svc


LIBS = -L"../../AVC/h264_baseline_decoder/lib_baseline" -L"../../AVC/h264_main_decoder/lib_main" -L"../lib_svc" -L"../bin/" -L"../lib_utils" -lviewsdl -lSDL -lsvc -lmain -lbaseline -lutils
ifeq ($(DEBUG),yes)
	CXFLAGS = -g
else	
	CXFLAGS = -O3 -fomit-frame-pointer -frerun-loop-opt -fgcse -fexpensive-optimizations -finline-functions -mmmx -msse -msse2 -msse3 -fstrict-aliasing
endif

export CXFLAGS

CFLAGS = $(INCS) -DDISPLAY $(CXFLAGS) -DHAVE_SDL -D_LOG_


RM = rm -f
.PHONY: all all-before all-after clean clean-custom


all: all-before $(BIN) all-after

clean: clean-custom
	@(cd "../../AVC/h264_baseline_decoder/lib_baseline" && $(MAKE) clean)
	@(cd "../../AVC/h264_main_decoder/lib_main" && $(MAKE) clean)
	@(cd "../libview" && $(MAKE) clean)
	@(cd "../lib_svc" && $(MAKE) clean)
	@(cd "../lib_utils" && $(MAKE) clean)
	@(cd "../YuvPlayer" && $(MAKE) clean)
	${RM} $(OBJ) $(BIN)
	rm -rf obj

$(BIN): $(OBJ)
	@(cd "../../AVC/h264_baseline_decoder/lib_baseline" && $(MAKE))
	@(cd "../../AVC/h264_main_decoder/lib_main" && $(MAKE))
	@(cd "../lib_svc" && $(MAKE))
	@(cd "../libview" && $(MAKE))
	@(cd "../lib_utils" && $(MAKE))
	@(cd "../YuvPlayer" && $(MAKE))
	$(CC) $(LINKOBJ) -o $(BIN) $(LIBS)

obj/PC.o: PC.c
	mkdir -p obj
	$(CC) -c PC.c -o obj/PC.o $(CFLAGS)
obj/decoder_svc_layer_nal.o: 	decoder_svc_layer_nal.c
	$(CC) -c $< -o $@ $(CFLAGS)
obj/decoder_base_layer_nal.o: 	decoder_base_layer_nal.c
	$(CC) -c $< -o $@ $(CFLAGS)
obj/read_video.o:		read_video.c
	$(CC) -c $< -o $@ $(CFLAGS)
obj/cal_time.o:			cal_time.c
	$(CC) -c $< -o $@ $(CFLAGS)
obj/write_yuv_file.o:		write_yuv_file.c
	$(CC) -c $< -o $@ $(CFLAGS)
