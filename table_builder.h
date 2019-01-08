#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include "block.h"
struct iterator {
	char *key;
	char *value;
	int id;
};

struct file_meta_data {
	int refs;
	int allowed_seeks;          // Seeks allowed until compaction
	uint64_t number;
	uint64_t file_size;         // File size in bytes
	struct internalkey_t *smallest;       // Smallest internal key served by table
	struct internalkey_t *largest;        // Largest internal key served by table
};
struct filter_block_builder{
	int op;
};

struct table_rep { 
	struct options_t *options; // 数据options.
	struct options_t *index_block_options; // index options.
	char  filename[10]; // sstable文件.
	char  file_size; // sstable文件.
	uint64_t offset; // 当前向这个file写入了多少数据.
	uint64_t cur_offset; // 当前向这个file写入了多少数据.
	int status; // file操作返回的status.
	struct block_builder *data_block; // data block.
	struct meta_block *meta_block; // index block.
	struct index_block *index_block; // index block.
	struct meta_index_block *meta_index_block; // index block.
	struct properities_block *prop_block; // index block.
	char *last_key;
	int64_t num_entries; // 已经插入了多少个kv.
	int64_t data_block_count; // 已经插入了多少个kv.
	bool closed;	        // Either Finish() or Abandon() has been called.
	struct filter_block_builder* filter_block;
	bool pending_index_entry; // 是否刚才调用了Finish.
	struct block_handle *pending_handle;  // Handle to add to index block
	uint64_t raw_value_size;
	uint64_t raw_key_size;
	uint64_t column_family_id;
	char column_family_name[NAME_LEN]; // sstable文件.
	struct filter_policy *filter_policy;
	char filter_policy_name[NAME_LEN];
	struct user_comparator *user_comparator;
	char comparator_name[NAME_LEN];
	struct merge_operator *merge_operator;
	char merge_operator_name[NAME_LEN];
	struct prefix_extractor *prefix_extractor;
	char prefix_extractor_name[NAME_LEN];
};


extern int write_block(struct table_rep *rep,struct block_builder *data_builder,struct block_handle *handle);
extern slice_t *block_builder_finish(struct block_builder *block);
