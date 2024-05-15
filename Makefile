


CFLAGS=-lncurses

NAME=tuip

SRC_FILES=main.c
OUT_FILE=$(NAME).so

INSTALL_DIR=/opt/tuip

.PHONEY: build clean install uninstall

all: build

build: ${SRC_FILES}

	${CC} ${SRC_FILES} -o${OUT_FILE} ${CFLAGS}

clean:

	rm ${OUT_FILE} 

install: build

	mkdir -p $(INSTALL_DIR)
	cp -f $(OUT_FILE) $(INSTALL_DIR)/$(NAME)
	chmod 755 $(INSTALL_DIR)/$(NAME)

	rm -f /usr/bin/$(NAME)
	ln $(INSTALL_DIR)/$(NAME) /usr/bin/$(NAME)

uninstall:
	rm -f /usr/bin/$(NAME)
	rm -f $(INSTALL_DIR)/$(NAME)
	rmdir $(INSTALL_DIR)

