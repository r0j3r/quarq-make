VPATH=../dependency:../parser

CC=gcc
CFLAGS=-std=gnu99 -Wall -MD -MP -g -O -flto -march=native
LDFLAGS=-O -g -flto -fuse-linker-plugin -march=native

dependency/command_queue.o : CFLAGS += -I../dependency -I../graph 
dependency/command_queue.o : dependency/command_queue.c 
node.o : CFLAGS += -I../dependency -std=gnu11
node.o : node.c
parse.o : CFLAGS += -I../parser -I../dependency
parse.o : parse.c
spooky-c.c:
spooky-c.h: 
spooky-c.o: spooky-c.h spooky-c.c
node: node.o spooky-c.o symbol.o parse.o
-include *.d
