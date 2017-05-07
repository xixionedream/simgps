C = gcc
CC = g++

CFLAG = -Wall -DOS_LINUX -std=c++11 -DWITH_NONAMESPACES

TIDE_ENV=../
TIDE_BIN=$(TIDE_ENV)bin
TIDE_INC=$(TIDE_ENV)include 
TIDE_SRC=$(TIDE_ENV)src
TIDE_LIB=$(TIDE_ENV)lib

OBJECT = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

TAR = tidedb-server
	
all: $(TAR) install
.PHONY: all install

$(OBJECT): %.o: %.cpp
	$(CC) $(CFLAG) -c $< -o $@ -I$(TIDE_INC) -I ./
$(TAR): $(OBJECT)
	$(CC) $(CFLAG) $(OBJECT) -o $@ -ldl -lpthread -L ../lib -lnetposa_license -lnpcrt  -lnplog -ltcps  -lmongoose  ../lib/libev.a ../lib/libhiredis.a ../lib/libjson.a

install:
	if test -d $(TIDE_BIN); then true; else mkdir $(TIDE_BIN); fi;
	mv $(TAR) ../bin
	rm -rf *.o
	
clean:
	rm -rf *.o
	rm -rf ./$(TAR)
	rm -rf $(TIDE_BIN)/$(TAR)
	
deapclean:
	rm -rf core*  *.xsd *.nsmap *.xml 
