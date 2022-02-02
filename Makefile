
CXX=gcc
#CXXFLAGS=-Wall -std=c11 -DDEBUG
CXXFLAGS=-Wall -std=c11

CFILES=config.c player.c othello.c
HFILES=config.h player.h othello.h
SRCFILES=$(CFILES) $(HFILES)

OFILES=config.o player.o othello.o

.SUFFIXES: .c 

all: othello

othello: $(OFILES)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $^ 

clean:
	rm *.o *.BAK

purge: clean
	rm othello 

