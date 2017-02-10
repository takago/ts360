# 2017/01/06 Takago Lab Makefile ver:1.00
CXX = g++
OPT = -O3 -fomit-frame-pointer -finline
CXXFLAGS = `pkg-config --cflags opencv` -Wno-unused-result ${OPT}
LDFLAGS = `pkg-config --libs opencv` -lglut -lGL -lGLU -lm

OBJS = ts360.o fish_translate.o panorama.o translate_movie.o init.o glcore.o

all:	ts360

ts360:	${OBJS}
	${CXX} -o $@ ${OBJS} ${LDFLAGS}

clean:
	rm -fr ${OBJS} ts360 tmp.mp4 *.jpg *.avi *~

demo0: ts360
	./ts360 ../theta.jpg
demo1: ts360
	./ts360 /tmp/tmp.avi
demo2: ts360
	./ts360 ../theta.avi -o /tmp/tmp.avi
demo3: ts360
	./ts360 -t /tmp/tmp.avi
demo4: ts360
	./ts360 -c 0
demo5: ts360
	./ts360 -s 192.168.19.150:8080  


dep:
	${CXX} -MM -w *.c *.h > dependencies

.cpp.o    :
	${CXX} -c $< -o $@ ${CXXFLAGS}
.c.o    :
	${CXX} -c $< -o $@ ${CXXFLAGS}

include dependencies
