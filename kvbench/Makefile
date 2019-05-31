OPT ?= -O2 -D__ROCKS_BENCH
include  build_config.mk
INCLUDE  = -I./include
CFLAGS   += $(INCLUDE) $(PLATFORM_CCFLAGS) $(OPT)
CXXFLAGS += $(INCLUDE) $(PLATFORM_CXXFLAGS) $(OPT)
LDFLAGS  += $(PLATFORM_LDFLAGS)
LIBS += $(PLATFORM_LIBS)
LIBOBJECTS = $(SOURCES:.cc=.o)

PROGRAMS = shannondb_bench
default: shannondb_bench
shannondb_bench: couch_bench.o $(LIBOBJECTS)
	$(CXX) $(LDFLAGS) couch_bench.o $(LIBOBJECTS) -o $@ $(LIBS)

clean:
	-rm -f  ./*.o */*.o */*/*.o *_bench

