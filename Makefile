PUB_BIN = ./bin
PUB_LIB = ./lib
PUB_OUTPUT = ./TideDB

.PHONY: bin pub src 

all: bin lib src 

bin:
	if test -d $(PUB_BIN); then true; else mkdir $(PUB_BIN); fi;

lib:
	if test -d $(PUB_LIB); then true; else mkdir $(PUB_LIB); fi;

src:
	$(MAKE) -C src
	

clean:
	$(MAKE) clean -C src
	rm -rf $(PUB_BIN)

