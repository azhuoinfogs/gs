SOURCES=utils/avltree.cc utils/crc32.cc utils/iniparser.cc utils/keygen.cc utils/keyloader.cc utils/memleak.cc utils/memory.cc utils/stopwatch.cc utils/thpool.cc utils/zipfian_random.cc wrappers/couch_shannondb.cc
CC=cc
CXX=g++
PLATFORM=
PLATFORM_LDFLAGS=
PLATFORM_CCFLAGS=
PLATFORM_CXXFLAGS=  -std=c++11
PLATFORM_LIBS= -lshannondb_cxx -lpthread -lnuma
PLATFORM_SHARED_CFLAGS=-fPIC
PLATFORM_SHARED_EXT=so
PLATFORM_SHARED_LDFLAGS=-shared -Wl,-soname -Wl,
PLATFORM_SHARED_VERSIONED=true
