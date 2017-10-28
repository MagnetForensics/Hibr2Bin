CC=gcc
CXX=g++
RM=rm -f
INCDIR=-I./
CXXFLAGS=-std=c++11 $(INCDIR)

SRCS=precomp.cpp CommonLibLight/FileContext.cpp CommonLibLight/Misc.cpp Disk.cpp Hiberfil.cpp MemoryBlocks.cpp Hibr2Bin.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: Hibr2Bin

Hibr2Bin: $(OBJS)
	$(CXX) $(CXXFLAGS) -o Hibr2Bin $(OBJS)

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) tool