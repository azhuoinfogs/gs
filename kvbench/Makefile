######################################
#
#######################################
TARGET  := shannondb_bench rocksdb_bench
CC      := gcc
CXX     := g++
SHANNON_SOURCE  := $(wildcard *.cc) $(wildcard utils/*.cc) $(wildcard wrappers/couch_shannondb.cc)
SHANNON_OBJS    := $(patsubst %.cc,%.o,$(patsubst %.cc,%.o,$(SHANNON_SOURCE)))
ROCKS_SOURCE  := $(wildcard *.cc) $(wildcard utils/*.cc) $(wildcard wrappers/couch_rocksdb.cc)
ROCKS_OBJS    := $(patsubst %.cc,%.o,$(patsubst %.cc,%.o,$(ROCKS_SOURCE)))
SHANNON_LIBS    :=  -lshannondb_cxx -lpthread -lnuma
ROCKS_LIBS    :=  -lrocksdb -lpthread -lnuma
LDFLAGS := -std=c++11
DEFINES :=
INCLUDE := -I. -I./include
CFLAGS  := -g -O2 $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS) $(LDFLAGS)

.PHONY : everything objs clean veryclean rebuild
everything : $(TARGET)
	  
all : $(TARGET)
	  
objs : $(SHANNON_OBJS) $(ROCKS_OBJS)
	  
rebuild: veryclean everything
clean :
		rm -fr *.o */*.o
		rm -fr *_bench
veryclean : clean
	    rm -fr $(TARGET)
shannondb_bench: $(SHANNON_OBJS)
	    $(CXX) $(CXXFLAGS) -o $@ $(SHANNON_OBJS) $(SHANNON_LIBS)
rocksdb_bench: $(ROCKS_OBJS)
	    $(CXX) $(CXXFLAGS) -o $@ $(ROCKS_OBJS) $(ROCKS_LIBS)
