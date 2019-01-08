#include<fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define DEBUG_CONFIG
#ifdef DEBUG_CONFIG
#define DEBUG(format, arg...) \
	printf("%s(), line=%d: " format, __FUNCTION__, __LINE__, ##arg)
#else
#define DEBUG(format, arg...)
#endif /* end of #ifdef DEBUG_CONFIG */
typedef signed char           int8_t;
typedef signed short          int16_t;
typedef signed int            int32_t;
typedef long int              int64_t;
typedef unsigned char         uint8_t;
typedef unsigned short        uint16_t;
typedef unsigned int          uint32_t;
typedef __ssize_t ssize_t;
#define  bool int
#define true 1
#define false 0
typedef uint64_t SequenceNumber;
#define BLOCK_SIZE 4096

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)                                                                                                                                                                      
#endif
struct slice{
	char *data;
	size_t size;
};
typedef struct slice slice_t;
static const SequenceNumber kMaxSequenceNumber =
    ((0x1ull << 56) - 1);

enum ValueType {
	kTypeDeletion = 0x0,
	kTypeValue = 0x1
};
static const enum ValueType kValueTypeForSeek = kTypeValue;
struct internalkey_t{
	slice_t *user_key;
	SequenceNumber sequence;
	enum ValueType type;
};
//extern void AppendInternalKey(std::string* result,
  //                            const ParsedInternalKey& key);
//extern bool ParseInternalKey(const Slice& internal_key,
  //                           ParsedInternalKey* result);
//void InternalKeyDecodeFrom(const Slice& s) { rep_.assign(s.data(), s.size());}



extern int varint_length(uint64_t v);
extern size_t put_fixed32(char* dst, uint32_t value);
extern size_t put_fixed64(char* dst, uint64_t value);
extern size_t put_varint64(char *dst, uint64_t v);
extern size_t put_varint32(char *dst, uint32_t v);
#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))
extern uint32_t crc32c_extend(uint32_t crc,char* buf, size_t size);
extern uint32_t crc32c_value(char* data, size_t n);
extern uint32_t crc32c_mask(uint32_t crc);
extern uint32_t crc32c_unmask(uint32_t masked_crc);
extern char *SSTTableFileName(char *name, uint64_t number);
extern char *TableFileName(char *name, uint64_t number);
extern void get_filesize(char *filename, size_t *file_size);
extern int write_raw(int fd, const char* p, size_t n);
extern void encode_fixed32(char* buf, uint32_t value);
extern struct slice *new_slice(char *str, size_t n);
extern struct internalkey_t  *internal_key(char *str, int n);
extern char *append_internalkey(struct internalkey_t *key);
extern size_t internal_key_size(struct internalkey_t *key);
extern struct slice *extract_userkey(struct slice *internal_key);
