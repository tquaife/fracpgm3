CC = gcc

CFLAGS = -Wall -g -static -O3
LIBS = -lm 
INCLS = -I./

VERSION=fracpgm3

OBJECTS=${VERSION}.o

fracpgm:	$(OBJECTS) ${VERSION}.c makefile 
		${CC} ${CFLAGS} -o ${VERSION} ${OBJECTS} ${INCLS} ${LIBS}

.c.o: $<
		$(CC) ${INCLS} $(CFLAGS) -c $<

clean:
		\rm -f *.o *~ *%

