#
#  Establish the compilers

CC=gcc
CXX=g++
CXXLD=g++
CCLD=gcc

CXXFLAGS=-g -I. -I$(DAQINC) -pthread -fPIC
CCFLAGS=$(CXXFLAGS)

ifdef LEGACY_MODE
	CXXFLAGS+= -DLEGACY_MODE
endif

LDFLAGS=-Wl,-rpath=$(PWD)

ROOTCFLAGS   := $(shell ${ROOTSYS}/bin/root-config --cflags)
ROOTGLIBS    := $(shell ${ROOTSYS}/bin/root-config --glibs)
ROOTLDFLAGS  := $(shell ${ROOTSYS}/bin/root-config --ldflags)

LIBS  = -lm $(ROOTGLIBS) -L$(DAQLIB) -L$(shell pwd) -lddaschannel

CXXFLAGS+=$(ROOTCFLAGS)
O_FILES = createEvent.o

all: libddaschannel.so runConverter

runConverter: $(O_FILES) libddaschannel.so
	$(CXXLD) -o runConverter $(O_FILES) $(LDFLAGS) $(LIBS) 

ddaschannelDictionary.cpp: ddaschannel.h DDASEvent.h LinkDef.h
	rm -f ddaschannelDictionary.cpp ddaschannelDictionary.h
	$(ROOTSYS)/bin/rootcint -f $@ -c -p ddaschannel.h DDASEvent.h LinkDef.h 

ddaschannelDictionary.o: ddaschannelDictionary.cpp
	$(CXX) -c -o $@ $(CXXFLAGS) $^

libddaschannel.so: ddaschannelDictionary.o ddaschannel.o DDASEvent.o
	$(CXX) -shared -Wl,-soname,$@ -o $@ $(ROOTGLIBS) $(CXXFLAGS) $^

depend:
	makedepend $(CXXFLAGS) *.cpp *.c

clean:
	rm -f *.o ddasdumper* libddaschannel.so ddaschannelDictionary*

print_env:
	@echo $(ROOTSYS)
	@echo $(ROOTCFLAGS)
	@echo $(ROOTGLIBS)
	@echo $(ROOTLDFLAGS)

