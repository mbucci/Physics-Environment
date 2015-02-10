PLATFORM = $(shell uname)


## Compilation flags
##comment out one or the other 
##debugging 
CFLAGS = -g 
##release
#CFLAGS = -O3 -DNDEBUG -Wdeprecated-declarations-
LDFLAGS=

CFLAGS+= -Wall

ifeq ($(PLATFORM),Darwin)
## Mac OS X
CFLAGS += -m64 -isystem/usr/local/include
LDFLAGS+= -m64 -lc -framework AGL -framework OpenGL -framework GLUT -framework Foundation
else
## Linux
CFLAGS += -m64
INCLUDEPATH  = -I/usr/include/GL/ 
LIBPATH = -L/usr/lib64 -L/usr/X11R6/lib
LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm  -lXmu -lXext -lXi
endif


CC = gcc -O3 -Wall $(INCLUDEPATH)
DEPS = header.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


PROGS = plan

default: $(PROGS)

plan: plan.o geom.o rtimer.o physics.o ball.o header.h
	$(CC) -o $@ plan.o geom.o rtimer.o physics.o ball.o $(LDFLAGS)

plan.o: plan.c rtimer.h 
	$(CC) -c $(INCLUDEPATH)  plan.c  -o $@

geom.o: geom.c header.h
	$(CC) -c $(INCLUDEPATH)  geom.c -o $@

physics.o: physics.c header.h
	$(CC) -c $(INCLUDEPATH) physics.c -o $@

ball.o: ball.c header.h
	$(CC) -c $(INCLUDEPATH) ball.c -o $@

rtimer.o: rtimer.h rtimer.c
	$(CC) -c $(INCLUDEPATH) rtimer.c -o $@
clean:	
	rm *.o
	rm plan