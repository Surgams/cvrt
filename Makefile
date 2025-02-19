CC =cc
CFLAGS= -O3 -Wall -Werror -Wextra 
CFLAGS_DEBUG= -g3 -Wall -Werror -Wextra 

RM=rm -f
MK=mkdir -p $(OUTPUT_DIR)

PROJECT_O=cvrt.o
PROJECT_B=cvrt
OUTPUT_DIR=bin
OBJECTS=cvrt.o ini.o processor.o options.o cmd.o filemgm.o versdts.o
SOURCES=cvrt.c ini.c processor.c options.c cmd.c filemgm.c versdts.c

VERSION=0.0.0
OUTPUT=$(PROJECT_B)

-include Makefile.depend

all: build 

build: depend PROJECT_O PROJECT_B 

debug:
		$(MK)
		$(CC)  -c $(SOURCES) $(CFLAGS_DEBUG)
		$(CC) -o $(OUTPUT_DIR)/$(PROJECT_B) $(OBJECTS) $(LIBS)

PROJECT_O:
		$(CC) -c $(SOURCES) $(CFLAGS)  

PROJECT_B:
		$(MK)
		$(CC) -o $(OUTPUT_DIR)/$(PROJECT_B) $(OBJECTS) $(LIBS)


clean:
		$(RM)  *.o Makefile.depend *.core *.bin bin/*

depend:
		cc -E -MM $(SOURCES) $(INCLUDE_DIR) > Makefile.depend

install: all
		install $(OUTPUT_DIR)/$(PROJECT_B) /usr/local/bin


deinstall:
		$(RM) /usr/local/bin/$(OUTPUT)

