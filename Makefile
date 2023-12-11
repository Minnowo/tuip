


CFLAGS=-lncurses

SRC_FILES=main.c
OUT_FILE=rn.out

all: main

main: ${SRC_FILES}

	${CC} ${SRC_FILES} -o${OUT_FILE} ${CFLAGS}

run: all 
	./${OUT_FILE}

clean:
	rm ${OUT_FILE} 