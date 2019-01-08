#include "util.h"
#include <stdint.h>
#define NAME_LEN  20

static const size_t kBlockTrailerSize = 5;
struct options_t {
	int block_restart_interval;
	int block_size;
	int compression;
	uint64_t column_family_id;
	char column_family_name[NAME_LEN]; // sstable文件.
};
struct block_builder{
	struct  options_t        *options;
	size_t offset;
	uint32_t restarts[32];    // Restart points
	int     restart_id;     // Number of entries emitted since restart
	int     counter;     // Number of entries emitted since restart
	bool    finished;    // Has Finish() been called?
	int     num_restarts;     // Number of entries emitted since restart
	char *last_key;
	char 	*buffer;      // Destination buffer
};

struct index_block {
	int     num_restarts;     // Number of entries emitted since restart
	uint32_t restarts[32];    // Restart points
	int     restart_id;     // Number of entries emitted since restart
	size_t  offset;
	char	*buffer;
};
struct meta_index_block {
	int     num_restarts;     // Number of entries emitted since restart
	uint32_t restarts[32];    // Restart points
	int     restart_id;     // Number of entries emitted since restart
	size_t  offset;
	char	*buffer;
};

struct meta_block {
};

struct properities_block {
	uint64_t raw_key_size;
	uint64_t raw_value_size;
	uint64_t data_size;
	uint64_t index_size;
	uint64_t index_partitions;
	uint64_t top_level_index_size;
	uint64_t index_key_is_user_key;
	uint64_t index_value_is_delta_encoded;
	uint64_t num_entries;
	uint64_t num_data_blocks;
	uint64_t num_deletions;
	uint64_t num_merge_operands;
	uint64_t num_range_deletions;
	uint64_t format_version;
	uint64_t filter_size;
	uint64_t fixed_key_len;
	uint64_t column_family_id;
	uint64_t creation_time;
	uint64_t oldest_key_time;	
	char filter_policy_name[NAME_LEN];
	char comparator_name[NAME_LEN];
	char merge_operator_name[NAME_LEN];
	char prefix_extractor_name[NAME_LEN];
	char property_collectors_names[100];
	char column_family_name[NAME_LEN];
	char compression_name[NAME_LEN];
	char *user_collected_properties;
	size_t  c_offset;
	char *readable_properties;
	size_t  r_offset;
	int     num_restarts;     // Number of entries emitted since restart
	uint32_t restarts[32];    // Restart points
	int     restart_id;     // Number of entries emitted since restart
	size_t  offset;
	char	*buffer;
	//struct rb_root *kvmap
};
struct index_block_record{
	char *key;
	struct block_handle *handle;
};

struct writable_file {
	char filename[10];
	int fd;
	size_t file_size;
};
enum { kMaxEncodedLength = 10 + 10 };
enum { kEncodedLength = 2*kMaxEncodedLength + 8};
static const uint64_t kTableMagicNumber = 0xdb4775248b80fb57ull;
static const uint64_t rTableMagicNumber = 0x88e241b785f4cff7ull;
struct block_handle {
	uint64_t offset;
	uint64_t size;
	int encode_max_length;
};
struct footer {
	struct block_handle *metaindex_handle;
	struct block_handle *index_handle;
	int encode_length;	
};

extern struct index_block *new_index_block();

extern int meta_index_block_add(struct meta_index_block *index_block,char *key,struct slice *value);
extern int block_add_record(struct block_builder *builder,const slice_t *key,const  slice_t *value);
extern struct slice *meta_index_block_finish(struct meta_index_block *block);
extern struct slice *index_block_finish(struct index_block *block);
extern struct slice *block_builder_finish(struct block_builder *block);
extern struct properities_block *new_properities_block();
extern struct meta_index_block *new_meta_index_block();
extern struct block_handle *new_block_handle();
extern struct slice * properities_block_finish(struct properities_block *block);
extern struct block_builder *new_block_builder();
extern void block_reset(struct block_builder *rep);
extern slice_t *new_slice(char *str, size_t n);
extern void block_builder_add(struct block_builder *builder,const slice_t *key, const slice_t *value);
extern void set_offset(struct block_handle *handle, uint64_t offset);
extern void set_size(struct block_handle *handle, uint64_t size);
extern size_t block_current_size(struct block_builder *block);
extern struct options_t *new_options(int interval, int size, int type);
extern char *footer_encode_to(struct footer *foot);
extern void set_metaindex_handle(struct footer *foot, struct block_handle *h);
extern struct footer *new_footer();
extern void set_index_handle(struct footer *foot, struct block_handle *h);

extern void set_last_key(char *last_key,char *key, size_t size);
extern char *block_handle_encode_to(struct block_handle *handle);
extern int writefile_append(char *file, char *data, size_t n);
extern int index_block_add(struct index_block *index_block,const slice_t *key, const slice_t *value);
extern size_t block_handle_length(struct block_handle *handle);

extern struct footer *new_rocksdb_footer();
extern char *rocksdb_footer_encode_to(struct footer *foot);
static char *kDataSize  = "rocksdb.data.size";
static char *kIndexSize = "rocksdb.index.size";
static char *kIndexPartitions = "rocksdb.index.partitions";
static char *kTopLevelIndexSize = "rocksdb.top-level.index.size";
static char *kIndexKeyIsUserKey = "rocksdb.index.key.is.user.key";
static char *kIndexValueIsDeltaEncoded = "rocksdb.index.value.is.delta.encoded";
static char *kFilterSize = "rocksdb.filter.size";
static char *kRawKeySize = "rocksdb.raw.key.size";
static char *kRawValueSize = "rocksdb.raw.value.size";
static char *kNumDataBlocks = "rocksdb.num.data.blocks";
static char *kNumEntries = "rocksdb.num.entries";
static char *kDeletedKeys = "rocksdb.deleted.keys";
static char *kMergeOperands = "rocksdb.merge.operands";
static char *kNumRangeDeletions = "rocksdb.num.range-deletions";
static char *kFilterPolicy = "rocksdb.filter.policy";
static char *kFormatVersion = "rocksdb.format.version";
static char *kFixedKeyLen = "rocksdb.fixed.key.length";
static char *kColumnFamilyId = "rocksdb.column.family.id";
static char *kColumnFamilyName = "rocksdb.column.family.name";
static char *kComparator = "rocksdb.comparator";
static char *kMergeOperator = "rocksdb.merge.operator";
static char *kPrefixExtractorName = "rocksdb.prefix.extractor.name";
static char *kPropertyCollectors = "rocksdb.property.collectors";
static char *kCompression = "rocksdb.compression";
static char *kCreationTime = "rocksdb.creation.time";
static char *kOldestKeyTime = "rocksdb.oldest.key.time";
static char *kVersion =  "rocksdb.external_sst_file.version";
static char *kGlobalSeqno = "rocksdb.external_sst_file.global_seqno";


static char *kPropertiesBlock = "rocksdb.properties";
static char *kPropertiesBlockOldName = "rocksdb.stats";
static char *kCompressionDictBlock = "rocksdb.compression_dict";
static char *kRangeDelBlock = "rocksdb.range_del";
