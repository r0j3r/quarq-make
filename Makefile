VPATH=..

CFLAGS=-MD -MP -O2 -flto -march=native
LDFLAGS=-O2 -flto -fuse-linker-plugin -march=native

dependency/command_queue.o : CFLAGS += -I../dependency -I../graph 
dependency/command_queue.o : dependency/command_queue.c 
dependency/node.o : CFLAGS += -I../dependency -std=gnu11
dependency/node.o : dependency/node.c 

-include */*.d
