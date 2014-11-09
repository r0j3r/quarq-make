VPATH=../dependency

CC=gcc
CFLAGS=-Wall -MD -MP -g -O -flto -march=native
LDFLAGS=-O -flto -fuse-linker-plugin -march=native

dependency/command_queue.o : CFLAGS += -I../dependency -I../graph 
dependency/command_queue.o : dependency/command_queue.c 
node.o : CFLAGS += -I../dependency -std=gnu11
node.o : node.c
spooky-c.c:
spooky-c.h: 
spooky-c.o: spooky-c.h spooky-c.c
node: node.o spooky-c.o
-include *.d
