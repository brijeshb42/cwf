LIBS=_libs
INCLUDES=_includes
LIBUVBUILDPATH=$(realpath libuv)

.PHONY: libuv http-parser map

prepare:
	-(git clone https://github.com/libuv/libuv.git)
	-(git clone https://github.com/nodejs/http-parser.git)
	-(git clone https://github.com/rxi/map.git)

mkdirs:
	-(mkdir -p $(LIBS) $(INCLUDES))

http-parser: mkdirs
	-(cd http-parser && make package)
	-(cp http-parser/*.a $(LIBS))
	-(cp http-parser/*.h $(INCLUDES))

pre-libuv:
	-(mkdir -p $(LIBUVBUILDPATH)/build)
	-(cd libuv && ./autogen.sh && ./configure --libdir=$(LIBUVBUILDPATH)/build && make)

libuv: mkdirs
	-(cd libuv && make install)
	-(cp libuv/include/*.h $(INCLUDES))
	-(cp libuv/build/*.a $(LIBS))

map: mkdirs
	-(cd map/src && $(CC) -c map.c && ar rcs libmap.a map.o)
	-(cp map/src/*.h $(INCLUDES))
	-(cp map/src/*.a $(LIBS))

clean:
	-(cd libuv && make clean)
	-(cd http-parser && make clean)
	-(cd map/src && rm -f *.a *.o)
	-(rm -rf $(LIBS) $(INCLUDES))

full-clean: clean
	-(rm -rf $(LIBUVBUILDPATH)/build)

setup: prepare pre-libuv

default: setup
