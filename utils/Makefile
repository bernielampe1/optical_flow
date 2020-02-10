FFMPEG_BASE := $(HOME)/projects/optical_flow/ffmpeg-0.6.1

CC := g++ -O3 -g -Wall -Wno-deprecated
CFLAGS := -c

BIN := computeOpticalFlow_imgs decodeStream graph_seg_img

INCLUDES := -I../src -I$(FFMPEG_BASE) -I$(FFMPEG_BASE)/libavformat -I$(FFMPEG_BASE)/libavcodec -I$(FFMPEG_BASE)/libswscale

LIBS := -L$(FFMPEG_BASE)/libavcodec -lavcodec -L$(FFMPEG_BASE)/libavutil -lavutil -L$(FFMPEG_BASE)/libavformat -lavformat -L$(FFMPEG_BASE)/libswscale -lswscale -lz -lm

all: $(BIN)

%: %.cpp
	$(CC) $^ $(INCLUDES) $(LIBS) -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBS) $< -o $@

.PHONY: clean
clean:
	rm -fr $(BIN) $(OBJS) *.dSYM

