#include "table_builder.h"
char key[100][256];
char value[100][256];

static int gkeylen;
static char gkey[256];
int gen_key_value(char *key, int *key_len, char *value, int *value_len, int position)
{
	int i = 0;
	int flage = 3;
	int ret = 0;
	for (i = gkeylen - 1; i >= 0; i--) {
		if (i == gkeylen - 1)
			gkey[i] = gkey[i] + 1;
		else
			gkey[i] = gkey[i] + flage;
		if (position == i)
			gkey[i] = gkey[i] + 1;
		if (gkey[i] > 'z') {
			flage = 1;
			gkey[i] = 'a';
		} else {
			flage = 0;
			break;
		}

	}
	strncpy(key, gkey, gkeylen);
	strncpy(value, gkey, gkeylen);
	*key_len = gkeylen;
	*value_len = gkeylen;
	if (flage == 1)
		ret = -1;
	return ret;
}
void print_key_value(char *key, int key_len, char *value, int value_len)
{
	char tkey[256];
	char tvalue[256];

	strncpy(tkey, key, key_len);
	strncpy(tvalue, value, value_len);
	tkey[key_len] = '\0';
	tvalue[value_len] = '\0';
	printf("%s:%s\n", tkey, tvalue);
}

void test_init_key(void)
{
	int i = 0;

	for (i = 0; i < gkeylen; i++)
		gkey[i] = 'a';
	gkey[i - 1] = gkey[i - 1] - 1;
}
void set_interval_key(int key_len)
{
	int keylen = key_len;
	int position = 1;
	gkeylen = keylen;
	test_init_key();
}

int valid(struct iterator *iter)
{
	if (iter->id >= 100)
		return 0;
	return 1;
}
void seek_to_first(struct iterator *iter)
{
	iter->id = 0;
	strcpy(iter->key,key[iter->id]);
	strcpy(iter->value,value[iter->id]);
}
void seek_to_last(struct iterator *iter)
{
	iter->id = 99;
	strcpy(iter->key,key[iter->id]);
	strcpy(iter->value,value[iter->id]);
}
void next(struct iterator *iter)
{
	iter->id++;
	strcpy(iter->key,key[iter->id]);
	strcpy(iter->value,value[iter->id]);
}
void prev(struct iterator *iter)
{
	iter->id--;
	strcpy(iter->key,key[iter->id]);
	strcpy(iter->value,value[iter->id]);
}

struct iterator *create_iterator()
{
	struct iterator *iter = (struct iterator *)malloc(sizeof(struct iterator));
	iter->key = malloc(10);
	iter->value = malloc(10);
	iter->id = 0;
}
struct file_meta_data *create_file_meta()
{
	struct file_meta_data *meta = malloc(sizeof(struct file_meta_data));
	memset(meta, 0, sizeof(struct file_meta_data));
	meta->number = 1;
	return meta;
}

int slice_compare(const slice_t *a, const slice_t *b)
{
	const size_t min_len = (a->size < b->size) ? (a->size) : (b->size);
	int r = memcmp(a->data, b->data, min_len);
	if (r == 0) {
		if (a->size < b->size) r = -1;
		else if (a->size > b->size) r = +1;
	}
	return r;
}

void FindShortestSeparator(slice_t* start, slice_t *limit)
{
	char *buf = NULL;
	char *start_data = start->data;
	char *limit_data = limit->data;
	size_t min_length = (start->size < limit->size) ? (start->size) : (limit->size);
	size_t diff_index = 0;
	while ((diff_index < min_length) && ((start_data)[diff_index] == limit_data[diff_index])) {
	  diff_index++;
	}
	if (diff_index >= min_length) {
	} else {
		uint8_t diff_byte = (uint8_t)((start_data)[diff_index]);
//		if (diff_byte < (uint8_t)(0xff) &&
//			diff_byte + 1 < (uint8_t)(limit_data[diff_index])) {
//			(start_data)[diff_index]++;
			char *temp = start_data;
			buf = malloc(diff_index + 2);
			memcpy(buf, temp, (start->size > (diff_index+1)?(diff_index+1):(start->size)));
			start->data = buf;
			buf[diff_index + 1]= '1';
			start->size++;
			free(temp);
//	  	}	
	}
}

int write_foot(struct table_rep *rep, struct block_handle *index_handle, struct block_handle *meta_handle)
{
		int ret = 0;
		struct footer *foot = new_rocksdb_footer();
		set_offset(foot->metaindex_handle, meta_handle->offset);
		set_size(foot->metaindex_handle, meta_handle->size);
		set_offset(foot->index_handle, index_handle->offset);
		set_size(foot->index_handle, index_handle->size);
		char *footer_encoding = rocksdb_footer_encode_to(foot);
		unsigned char * tmp = (unsigned char *)footer_encoding;
		DEBUG("foot %d--%d\n", index_handle->offset, index_handle->size);
		slice_t *input = new_slice(footer_encoding, foot->encode_length);
/*		char *magic_ptr = footer_encoding + foot->encode_length - 8;
		uint32_t magic_lo = decode_fixed32(magic_ptr);
		uint32_t magic_hi = decode_fixed32(magic_ptr + 4);
		uint32_t value = decode_fixed32(footer_encoding +foot->encode_length  + 1);
		DEBUG("checksum = %d\n", value);
		DEBUG("type = %d magic_hi=%x, magic_lo=%x\n",footer_encoding[0], magic_hi, magic_lo); 
*/
	  	ret = writefile_append(rep->filename, input->data, input->size);
/*		if (ret ==  0) {
			unsigned char trailer[kBlockTrailerSize];
			trailer[0] = 0;
			uint32_t crc = crc32c_value(input->data, input->size);
			crc = crc32c_extend(crc, trailer, 1);
			encode_fixed32(trailer+1, crc32c_mask(crc));
			int ret = writefile_append(rep->filename, trailer, kBlockTrailerSize);
			if (ret < 0) {
				DEBUG("writeblock failed\n");
				return -1;
			}
		} else {
			DEBUG("writeblock failed ret= %d\n", ret);
			return -1;
		}
*/
		return 0;
}

void filter_block_addkey(const slice_t *key)
{
	printf("filter add block key %s -%d\n", key->data, key->size);
}
int write_raw_block(struct table_rep *rep,const slice_t *block_contents, struct block_handle *handle, int type)
{
	int ret = 0;
	set_offset(handle, rep->cur_offset);
	set_size(handle, block_contents->size);
	printf("#####%d#####%d\n", block_contents->data[block_contents->size -5], block_contents->size);
	ret = writefile_append(rep->filename ,block_contents->data, block_contents->size);
	if (ret ==  0) {
		unsigned char trailer[kBlockTrailerSize];
		trailer[0] = 0;
		uint32_t crc = crc32c_value(block_contents->data, block_contents->size);
		crc = crc32c_extend(crc, trailer, 1);
		encode_fixed32(trailer + 1, crc32c_mask(crc));
		int ret = writefile_append(rep->filename, trailer, kBlockTrailerSize);
		if (ret < 0) {
			printf("writeblock failed\n");
			return -1;
		}
	} else {
		printf("writeblock failed ret= %d\n", ret);
		return -1;
	}
	rep->cur_offset += (block_contents->size + kBlockTrailerSize);
	rep->index_block->offset += ( kBlockTrailerSize);
	return 0;
}
int write_raw1_block(struct table_rep *rep,const slice_t *block_contents, struct block_handle *handle, int type)
{
	int ret = 0;
	set_offset(handle, rep->cur_offset);
	set_size(handle, block_contents->size);
	ret = writefile_append(rep->filename ,block_contents->data, block_contents->size);
	if (ret ==  0) {
		unsigned char trailer[kBlockTrailerSize];
		trailer[0] = 0;
		uint32_t crc = crc32c_value(block_contents->data, block_contents->size);
		crc = crc32c_extend(crc, trailer, 1);
		encode_fixed32(trailer + 1, crc32c_mask(crc));
		int ret = writefile_append(rep->filename, trailer, kBlockTrailerSize);
		if (ret < 0) {
			printf("writeblock failed\n");
			return -1;
		}

	} else {
		printf("writeblock failed ret= %d\n", ret);
		return -1;
	}
	rep->cur_offset += (block_contents->size + kBlockTrailerSize);
	block_reset(rep->data_block);
	return 0;
}
int write_raw2_block(struct table_rep *rep,const slice_t *block_contents, struct block_handle *handle, int type)
{
	int ret = 0;
	set_offset(handle, rep->cur_offset);
	set_size(handle, block_contents->size);
	ret = writefile_append(rep->filename ,block_contents->data, block_contents->size);
	if (ret ==  0) {
		unsigned char trailer[kBlockTrailerSize];
		trailer[0] = 0;
		uint32_t crc = crc32c_value(block_contents->data, block_contents->size);
		crc = crc32c_extend(crc, trailer, 1);
		encode_fixed32(trailer+1, crc32c_mask(crc));
		int ret = writefile_append(rep->filename, trailer, kBlockTrailerSize);
		if (ret < 0) {
			DEBUG("writeblock failed\n");
			return -1;
		}
	} else {
		DEBUG("writeblock failed ret= %d\n", ret);
		return -1;
	}
	rep->cur_offset += (block_contents->size + kBlockTrailerSize);
	return 0;
}
int write_raw3_block(struct table_rep *rep,const slice_t *block_contents, struct block_handle *handle, int type)
{
	int ret = 0;
	ret = writefile_append(rep->filename ,block_contents->data, block_contents->size);
	if(ret < 0){
		DEBUG("writeblock failed ret= %d\n", ret);
		return -1;
	}
	rep->cur_offset += (block_contents->size);
	return 0;
}

int write_metaindex_block(struct table_rep *rep,struct meta_index_block *metaindex_block, struct block_handle *handle)
{
	slice_t *raw = meta_index_block_finish(metaindex_block);
	slice_t *block_contents = NULL;
	int type  = rep->options->compression;
	switch (type) {
	    case 0:
		block_contents = raw;
     		break;
	    case 1:	
		block_contents = raw;
     		break;
	    default:
		DEBUG("unknow type=%d\n",type);
		break;
	}
	return write_raw2_block(rep, raw, handle, type);
}

static int kNoCompression = 0;
static int kSnappyCompression = 1;

int write_data_block(struct table_rep *rep,struct block_builder *block,struct block_handle *handle)
{
	slice_t *raw = block_builder_finish(block);
	slice_t *block_contents = NULL;
	int type  = rep->options->compression;
	switch (type) {
	    case 0:
		block_contents = raw;
     		break;
	    case 1:	
		block_contents = raw;
     		break;
	    default:
		DEBUG("unknow type=%d\n",type);
		break;
	}
	DEBUG("%d\n", rep->data_block->offset);
	return write_raw1_block(rep, raw, handle, type);
}

int write_index_block(struct table_rep *rep,struct index_block *block,struct block_handle *handle)
{
	DEBUG("%d\n", rep->index_block->offset);
	slice_t *raw = index_block_finish(block);
	slice_t *block_contents = NULL;
	int type  = rep->options->compression;
	switch (type) {
	    case 0:
		block_contents = raw;
     		break;
	    case 1:	
		block_contents = raw;
		type = kNoCompression;
     		break;
     	    default:
		DEBUG("unknow type=%d\n",type);
		break;
	}
	return write_raw_block(rep, raw, handle, type);
}

void build_properities_block(struct table_rep * rep, struct properities_block *props)
{
	props->column_family_id = rep->column_family_id;
	strcpy(props->column_family_name, rep->column_family_name);
	(rep->filter_policy != NULL) ? strcpy(props->filter_policy_name, rep->filter_policy_name):strcpy(props->filter_policy_name, "");
	(rep->user_comparator != NULL) ? strcpy(props->comparator_name, rep->comparator_name):strcpy(props->comparator_name, "nullptr");
	(rep->merge_operator != NULL) ? strcpy(props->merge_operator_name, rep->merge_operator_name):strcpy(props->merge_operator_name, "nullptr");
	strcpy(props->compression_name,"kNoCompression");
	(rep->prefix_extractor != NULL) ? strcpy(props->prefix_extractor_name, rep->prefix_extractor_name):strcpy(props->prefix_extractor_name, "nullptr");
//	char *property_collectors_names = "[]";
//	char *property_collectors_names = "[RegularKeysStartWithA, TablePropertiesTest, Rocksdb]";	
//	strcpy(props->property_collectors_names, property_collectors_names);
//	add_properities_collector_names(props, "TablePropertiesTest", "Rocksdb");
//	add_properities_collector_fix32_number(props, kVersion, 2);
//	add_properities_collector_fix64_number(props, kGlobalSeqno, 0);
	props->index_partitions  = 0;
	props->top_level_index_size = 0;
	props->index_value_is_delta_encoded = true;
	props->index_key_is_user_key = true;
	props->creation_time = 0;
	props->oldest_key_time = 0;	
	props->raw_key_size = 1;
	//props->raw_key_size = rep->raw_key_size;
	props->raw_value_size = rep->raw_value_size;
	props->data_size = rep->data_block->offset + kBlockTrailerSize;
	props->index_size = rep->index_block->offset + kBlockTrailerSize;
	props->num_entries = rep->num_entries;
	props->num_deletions = 0;
	props->num_merge_operands = 0;
	props->num_range_deletions = 0;
	props->num_data_blocks = rep->data_block_count;
	props->filter_size = 0;
	props->format_version = 0;
	props->fixed_key_len = 0;
}

int write_meta_properties_block(struct table_rep *rep,struct properities_block *props,struct block_handle *handle)
{
	build_properities_block(rep, props);
	slice_t *raw = properities_block_finish(props);
	slice_t *block_contents = NULL;
	int type  = rep->options->compression;
	switch (type) {
	    case 0:
		block_contents = raw;
     		break;
	    case 1:	
		block_contents = raw;
		type = kNoCompression;
     		break;
     	    default:
		DEBUG("unknow type=%d\n",type);
		break;
	}
	return write_raw2_block(rep, block_contents, handle, 0);
}
/*
int table_builder_finish(struct table_rep *rep)
{
	int ret = 1;
	table_flush(rep);
	struct 	block_handle filter_block_handle;
	struct block_handle *index_block_handle  = new_block_handle();
	struct block_handle *metaindex_block_handle  = new_block_handle();
	if (rep->filter_block) {
		//writeblock filter;
	}
	struct index_block *meta_index_block = new_index_block(rep->index_block_options);
	if (rep->filter_block != NULL) {

	}
	ret = write_block(rep, meta_index_block, metaindex_block_handle);
	if (ret > 0) {
	    if (rep->pending_index_entry) {
		FindShortestSeparator(rep->last_key, 6);		
		char* handle_encoding = block_handle_encode_to(rep->pending_handle);
		size_t encode_length =  block_handle_length(rep->pending_handle);
		index_block_add(rep->index_block, new_slice(rep->last_key,strlen(rep->last_key)), new_slice(handle_encoding, encode_length));
		rep->pending_index_entry = false;	
    	    }
	    ret = write_block(rep ,rep->index_block, index_block_handle);
	}
	if (ret > 0) {
		int meta_index_offset, meta_index_size, index_offset, index_size;
		struct footer *foot = new_footer();
		set_offset(foot->metaindex_handle, metaindex_block_handle->offset);
		set_size(foot->metaindex_handle, metaindex_block_handle->size);
		set_offset(foot->index_handle, index_block_handle->offset);
		set_size(foot->index_handle, index_block_handle->size);
		char *footer_encoding = footer_encode_to(foot);
		unsigned char * tmp = (unsigned char *)footer_encoding;
		slice_t input;
		set_slice(&input, footer_encoding, foot->encode_length);
		get_varint64(&input, &meta_index_offset);	
		get_varint64(&input, &meta_index_size);	
		get_varint64(&input, &index_offset);	
		get_varint64(&input, &index_size);	
		DEBUG("foot meta%d index%d meta_size%d--index_size%d\n", meta_index_offset, index_offset, meta_index_size, index_size);
		char *magic_ptr = footer_encoding + foot->encode_length - 8;
		uint32_t magic_lo = decode_fixed32(magic_ptr);
		uint32_t magic_hi = decode_fixed32(magic_ptr + 4);
		DEBUG("magic_hi=%x, magic_lo=%x\n",magic_hi, magic_lo); 
	  	ret = writefile_append(rep->filename, footer_encoding, foot->encode_length);
	  	if (ret) {
				rep->offset += foot->encode_length;
	  	}
	}
	return 0;
} 
*/
struct table_rep *new_table_builder(char *fname, struct options_t *options)
{
	struct table_rep *rep = malloc(sizeof(struct table_rep));
	rep->options = new_options(options->block_restart_interval, options->block_size, options->compression);
	rep->index_block_options = new_options(1, options->block_size, options->compression);
	rep->data_block = new_block_builder(options);
	rep->index_block = new_index_block();
	rep->meta_index_block = new_meta_index_block();
//	rep->meta_block = new_meta_block();
	rep->prop_block = new_properities_block();
	rep->pending_handle = new_block_handle();
	rep->filter_block =	NULL;
	rep->offset = 0;
	rep->cur_offset = 0;
	rep->pending_index_entry  = false;
	strcpy(rep->filename, fname);
	strcpy(rep->column_family_name, "testdb");
	rep->column_family_id = 0;
	rep->last_key = malloc(128);
	rep->filter_policy = NULL;
	rep->user_comparator = NULL;
	rep->merge_operator = NULL;
	rep->prefix_extractor = NULL;
	return rep;
}

void block_finish(struct table_rep *rep)
{
	int ret = write_data_block(rep,rep->data_block, rep->pending_handle);
	if (ret == 0) {
		rep->pending_index_entry = true;
		rep->data_block->finished = true;
		rep->data_block_count++;
	}

}
int table_builder_add(struct table_rep *rep,struct slice *key,struct slice *value) 
{
	int ret = 0 ;
		struct slice *user_key =  extract_userkey(key);
//// 确保按照顺序操作.
	if (rep->num_entries > 0) { // 确保按照顺序操作.
		assert(slice_compare(key, new_slice(rep->last_key, strlen(rep->last_key))) > 0);
	}
	if (rep->pending_index_entry) {	
		slice_t *last_key = new_slice(rep->last_key, strlen(rep->last_key));
		FindShortestSeparator(last_key, user_key);	
		struct internalkey_t *key = internal_key(last_key->data, last_key->size);
		char *buf = append_internalkey(key);
		int size = internal_key_size(key);
		slice_t *slice_key = new_slice(buf, size);
		char  *handle_encoding =  block_handle_encode_to(rep->pending_handle);	
		//DEBUG("index block %d--%s--%d--%d--%s --%d--%d--%d--%d\n", rep->cur_offset, user_key->data,rep->pending_handle->offset, rep->pending_handle->size, last_key->data, slice_key->size, block_handle_length(rep->pending_handle), rep->data_block->num_restarts, rep->data_block->offset);
		index_block_add(rep->index_block, slice_key, new_slice(handle_encoding, block_handle_length(rep->pending_handle)));
		DEBUG("offset %d\n", rep->index_block->offset);
		rep->pending_index_entry = false;
		if (rep->data_block_count == 2) {
			ret = write_meta_properties_block(rep, rep->prop_block, rep->pending_handle);
			if (ret < 0) {
				printf("table flush  write properities_block failed\n");
			}	
			DEBUG("meta block %d--%d--%d\n", rep->cur_offset,rep->pending_handle->offset, rep->pending_handle->size);
			char  *handle_encoding =  block_handle_encode_to(rep->pending_handle);	
			meta_index_block_add(rep->meta_index_block, kPropertiesBlock, new_slice(handle_encoding, block_handle_length(rep->pending_handle)));
			return -1;
		}
  	}
	if (rep->filter_block != NULL) {
		filter_block_addkey(key);	
	}
	// 更新last_key并且插入data block.
	set_last_key(rep->last_key, user_key->data, user_key->size);
	rep->num_entries++;
	DEBUG("%d--%d--%d\n",rep->num_entries, rep->data_block->offset, key->size);
	block_add_record(rep->data_block, key, value);
	const size_t estimated_block_size = block_current_size(rep->data_block);
	DEBUG("%d--%d---%d\n",rep->num_entries, rep->data_block->offset, estimated_block_size);
	if (estimated_block_size >= 1000) {
		rep->pending_index_entry = true;
/*		rep->data_block->restart_id++;
		rep->data_block->num_restarts++;
		rep->data_block->restarts[rep->data_block->restart_id] = rep->data_block->offset;
*/
		block_finish(rep);
	}
	return 0;
}
void table_flush(struct table_rep *rep)
{
	struct block_handle *meta_handle = new_block_handle();
	struct block_handle *index_handle = new_block_handle();
	struct block_handle *handle = new_block_handle();
	int ret = 0;	
/*	if (rep->closed != true) {
		ret = write_data_block(rep,rep->data_block, handle);
		if (ret < 0) {
			DEBUG("table flush  write data block failed\n");
		}
		FindShortestSeparator(user_key->data, 6);	
		DEBUG("%s--%s\n", rep->last_key->data, user_key->data);
		InternalKey *key = internal_key(user_key->data, user_key->size);
		slice_t *slice_key = new_slice(key->data, key->size);
		index_block_add(rep->index_block, slice_key, handle);
	}
*/
	DEBUG("%d--%d--%d\n", rep->index_block->offset, rep->data_block->offset, rep->cur_offset);
	ret = write_metaindex_block(rep, rep->meta_index_block, meta_handle);
	if (ret < 0) {
		printf("table flush  write indexblock failed\n");
	}
	ret = write_index_block(rep, rep->index_block, index_handle);
	if (ret < 0) {
		printf("table flush  write indexblock failed\n");
	}
	DEBUG("index_offset=%d size = %d\n", index_handle->offset,index_handle->size);
	DEBUG("meta_offset=%d size = %d\n", meta_handle->offset,meta_handle->size);
	ret = write_foot(rep, index_handle, meta_handle);
	if (ret < 0) {
		DEBUG("table flush  write foot failed\n");
	}
}

int build_table(char* dbname, struct options_t *option, struct iterator *iter, struct file_meta_data* meta)
{
	int meta_index_offset, meta_index_size, index_offset, index_size;
	FILE *fp;
	size_t file_size;
	meta->file_size = 0;
	int ret = 0;
	seek_to_first(iter);
	//char *fname = TableFileName(dbname, meta->number); // 注意number这个字段含义.
	char *fname = "file2.sst"; // 注意number这个字段含义.
	if (valid(iter)) {
		fp = fopen(fname,"w");
		if (!fp) {
			printf("error %s %d %s\n",__func__, __LINE__, fname);
			return -1;
		}
	}
	struct table_rep *rep = new_table_builder(fname, option);
	struct index_block *index_block = new_index_block();
	struct block_handle *handle = new_block_handle();
		set_offset(handle, 1);
		set_size(handle, 1);
	meta->smallest = internal_key(iter->key,strlen(iter->key)); 
	for (; valid(iter); next(iter)) {
		struct internalkey_t *key = internal_key(iter->key, strlen(iter->key));
		int size = internal_key_size(key);
		char *buf = append_internalkey(key);
		struct slice *slice_key = new_slice(buf, size);
		struct slice *value = new_slice(iter->value, strlen(iter->value));
		ret = table_builder_add(rep, slice_key, value);
		if (ret < 0) break;
	}
	for (int i = 0 ; i < rep->data_block->num_restarts; i++) {
		printf(" %d data_block->restarts= %d -%d -%d\n",i, rep->data_block->restarts[i], rep->data_block->num_restarts, rep->data_block->options->block_restart_interval);
	}
	DEBUG("data_block %d -%d\n", rep->data_block->offset, rep->data_block->num_restarts);
	table_flush(rep);
	DEBUG("entries, data_block %d -- %d\n", rep->data_block->offset, rep->num_entries);
	meta->largest = internal_key(iter->key,strlen(iter->key)); 
/*	ret = table_builder_finish(builder);
	if (ret < 0) {
		printf("table_builder_finish() failed\n");
		return ret;
	}
*/
	get_filesize(fname, &file_size);
	printf("file_size %d\n", file_size);
//	meta->file_size = file_size;
	return 0;	
}

int main()
{
	int key_len = 0;
	int ret = 0;
	int position = 0;
	set_interval_key(8);
	int value_len = 0;
	for (int i = 0; i < 100; i++) {
		ret = gen_key_value(key[i], &key_len, value[i], &value_len, 1);
		if (ret < 0) {
			printf("gen_key_value failed%\n");
		}
	}
	print_key_value(key[0], key_len, value[0], value_len);
	struct iterator *iter =	create_iterator();
	struct file_meta_data *meta = create_file_meta();
	struct options_t *options = new_options(16, 1000, 0);
/*	while(valid(iter)) {
		print_key_value(iter->key, strlen(iter->key), iter->value, strlen(iter->value));
		next(iter);
	}
*/
	build_table("leveldb", options, iter, meta);
//	build_table("rocksdb", options, iter, meta);
}

