CC=clang
OPTIMIZATION?=-O2
NODEPS:=clean
DEPENDENCY_TARGETS=map http-parser libuv

# STD=-std=c99 -pedantic
STD=-pedantic
WARN=-Wall -W -Wno-missing-field-initializers
OPT=$(OPTIMIZATION)

PREFIX?=..
INSTALL_BIN=$(PREFIX)/bin
INSTALL=install

DEBUG=-g -ggdb
FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS)
FINAL_LIBS=-lm -ldl

FINAL_DEPS+= -I$(PREFIX)/deps/_includes

BIN_NAME=hello-main
BIN_OBJ=main.o request.o server.o

default: all

.make-prerequisites:
	(cd ../deps && $(MAKE) $(DEPENDENCY_TARGETS))

Makefile.dep:
	$(CC) -MM *.c > Makefile.dep 2> /dev/null || true

.PHONY: Makefile.dep
.PHONY: all
.PHONY: .make-prerequisites

%.o: %.c .make-prerequisites
	$(CC) $(FINAL_DEPS) -c $<

clean:
	-(rm -rf Makefile.dep *.o $(BIN_NAME) $(INSTALL_BIN) Makefile.dep)

distclean: clean
	-(cd ../deps && $(MAKE) clean)

$(BIN_NAME): $(BIN_OBJ)
	@echo $(FINAL_LIBS)
	$(CC) $(FINAL_CFLAGS) $(FINAL_DEPS) -o $@ $^ ../deps/_libs/libhttp_parser.a ../deps/_libs/libmap.a ../deps/_libs/libuv.a $(FINAL_LIBS)
	@mkdir -p $(INSTALL_BIN)
	cp $(BIN_NAME) $(INSTALL_BIN)
	rm $(BIN_NAME)
	@echo "Installed the binary to $(INSTALL_BIN)/$(BIN_NAME)"

all: $(BIN_NAME)
	@echo "Build Done"
