#include "block.h"
void set_offset(struct block_handle *handle, uint64_t offset)
{
	handle->offset = offset;
}
void set_size(struct block_handle *handle, uint64_t size)
{
	handle->size = size;
}

struct options_t *new_options(int interval, int size, int type)
{
	struct options_t *option = malloc(sizeof(struct options_t));
	option->block_restart_interval = interval;
	option->block_size = size;
	option->compression = type;
	return option;
}
/*** slice ***/

void set_last_key(char *last_key,char *key, size_t size)
{
	memcpy(last_key, key, size);
}
struct block_builder *new_block_builder(struct options_t *options)
{
	struct block_builder *builder= malloc(sizeof(struct block_builder));
	builder->options = new_options(options->block_restart_interval, options->block_size, options->compression);
	builder->buffer = malloc(BLOCK_SIZE);
	builder->finished = false;
	builder->num_restarts= 1;
	builder->last_key = malloc(20);
	memset(builder->buffer, 0, BLOCK_SIZE);
	memset(builder->last_key, 0, 20);
	return builder;
} 
void pput_var32(struct properities_block *builder, uint32_t v)
{
	size_t offset = put_varint32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

void mput_var32(struct meta_index_block *builder, uint32_t v)
{
	size_t offset = put_varint32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

void mput_fix32(struct meta_index_block *builder, uint32_t v)
{
	size_t offset = put_fixed32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}


void iput_var32(struct index_block *builder, uint32_t v)
{
	size_t offset = put_varint32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

void iput_fix32(struct index_block *builder, uint32_t v)
{
	size_t offset = put_fixed32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

void put_var32(struct block_builder *builder, uint32_t v)
{
	size_t offset = put_varint32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

void put_fix32(struct block_builder *builder, uint32_t v)
{
	size_t offset = put_fixed32(builder->buffer + builder->offset, v);
	builder->offset += offset;
}

size_t block_current_size(struct block_builder *block)
{
       return (block->offset + 32 * sizeof(uint32_t) + sizeof(uint32_t));  // Restart array
}  


void append_buffer(struct block_builder *builder, char *buffer, size_t size)
{
	memcpy(builder->buffer + builder->offset, buffer, size);
	builder->offset += size;
}

char *block_handle_encode_to(struct block_handle *handle)
{
	char* buf = malloc(handle->encode_max_length);
	size_t offset = 0;
	offset = put_varint64(buf, handle->offset);
	put_varint64(buf + offset, handle->size);
	return buf;
}

size_t block_handle_length(struct block_handle *handle)
{
	size_t offset_length = varint_length(handle->offset);
	size_t size_length = varint_length(handle->size);
	return size_length + offset_length;
}

size_t index_block_add_record(struct index_block *index_block,const slice_t *key, const slice_t *value)
{
	size_t shared = 0;
	size_t offset = 0;
	size_t index_offset = index_block->offset;
	size_t value_size = 0;
	offset = put_varint32(index_block->buffer + index_offset, 0);
	DEBUG("offset = %d\n", offset);
	index_offset += offset;
	offset = put_varint32(index_block->buffer + index_offset, key->size);
	DEBUG("offset = %d\n", index_offset);
	index_offset += offset;
	offset = put_varint32(index_block->buffer + index_offset, value->size);
	DEBUG("offset = %d\n", index_offset);
	index_offset += offset;
	memcpy(index_block->buffer + index_offset,key->data, key->size);
	DEBUG("offset = %d--%s\n", key->size, key->data);
	DEBUG("offset = %d\n", index_offset);
	index_offset += key->size;
	memcpy(index_block->buffer + index_offset, value->data, value->size);
	DEBUG("offset = %d\n", value->size);
	DEBUG("offset = %d\n", index_offset);
	index_offset += value->size;
	return index_offset;
}

size_t properity_block_add_restart(struct properities_block *index_block)
{
	size_t offset = 0;
	size_t i = 0;
	size_t index_offset = index_block->offset;
	for(i = 0; i < index_block->restart_id; i++ ) {
		offset = put_fixed32(index_block->buffer + index_offset, index_block->restarts[i]);
		index_offset += offset;
	}
	offset = put_fixed32(index_block->buffer + index_offset, index_block->restart_id);
	index_offset += offset;
	index_block->offset = index_offset;
	return offset;
}

size_t index_block_add_restart(struct index_block *index_block)
{
	size_t offset = 0;
	size_t i = 0;
	size_t index_offset = index_block->offset;
	for(i = 0; i < index_block->restart_id; i++ ) {
		offset = put_fixed32(index_block->buffer + index_offset, index_block->restarts[i]);
		index_offset += offset;
	}
	offset = put_fixed32(index_block->buffer + index_offset, index_block->restart_id);
	index_offset += offset;
	index_block->offset = index_offset;
	return offset;
}

void data_block_add_restart(struct block_builder *data_block)
{
	size_t offset = 0;
	size_t i = 0;
	size_t index_offset = data_block->offset;
	for(i = 0; i < data_block->num_restarts; i++ ) {
		offset = put_fixed32(data_block->buffer + index_offset, data_block->restarts[i]);
		index_offset += offset;
	}
	offset = put_fixed32(data_block->buffer + index_offset,data_block->num_restarts);
	index_offset += offset;
	data_block->offset = index_offset;
}

int index_block_add(struct index_block *index_block,const slice_t *key, const slice_t *value)
{
	size_t offset = 0;
	iput_var32(index_block, 0);
	iput_var32(index_block, key->size);
	iput_var32(index_block, value->size);
	memcpy(index_block->buffer + index_block->offset, key->data, key->size);
	index_block->offset += key->size;
	memcpy(index_block->buffer + index_block->offset, value->data, value->size);
	index_block->offset += value->size;
	index_block->restart_id++;
	index_block->num_restarts++;
	index_block->restarts[index_block->restart_id] = index_block->offset;
	return 0;
}

int meta_index_block_add(struct meta_index_block *index_block,char *key,struct slice *value)
{
	size_t offset = 0;
	mput_var32(index_block, 0);
	mput_var32(index_block, strlen(key));
	mput_var32(index_block, value->size);
	memcpy(index_block->buffer + index_block->offset, key,strlen(key));
	index_block->offset += strlen(key);
	memcpy(index_block->buffer + index_block->offset, value->data, value->size);
	index_block->offset += value->size;
	index_block->restart_id++;
	index_block->num_restarts++;
	index_block->restarts[index_block->restart_id] = index_block->offset;
	return 0;
}

struct index_block *new_index_block()
{
	struct index_block *builder= malloc(sizeof(struct index_block));
	builder->buffer = malloc(BLOCK_SIZE);
	memset(builder->buffer, 0, BLOCK_SIZE);
	memset(builder->restarts, 0, 32 * sizeof(uint32_t));
	builder->num_restarts = 1;
	builder->restart_id = 0;
	builder->offset = 0;
	return builder;
} 
struct meta_index_block *new_meta_index_block()
{
	struct meta_index_block *builder= malloc(sizeof(struct meta_index_block));
	builder->buffer = malloc(BLOCK_SIZE);
	memset(builder->buffer, 0, BLOCK_SIZE);
	memset(builder->restarts, 0, 32 * sizeof(uint32_t));
	builder->num_restarts = 1;
	builder->restart_id = 0;
	builder->offset = 0;
	return builder;
} 

int block_add_record(struct block_builder *builder,const slice_t *key,const  slice_t *value)
{
	size_t shared = 0;
	if (builder->finished) {
		DEBUG("%d--%s--%d--%s\n",key->size, key->data, value->size, value->data);
		put_var32(builder, 0);
		put_var32(builder, key->size);
		put_var32(builder, value->size);
		append_buffer(builder, key->data, key->size);
		append_buffer(builder, value->data, value->size);
		set_last_key(builder->last_key, key->data, key->size);
		builder->finished = false;
		builder->counter++;
		return 0;
	}
	slice_t *last_key_piece = new_slice(builder->last_key, strlen(builder->last_key));
	if (builder->counter < 16) {
		const size_t min_length = min(last_key_piece->size, key->size);
		while ((shared < min_length) && (last_key_piece->data[shared] == key->data[shared])) {
			shared++;
		}
	} else {
		DEBUG("block  add restarts %d--%d--%d--%d\n",builder->restart_id, builder->num_restarts, builder->offset, builder->counter);
		builder->restart_id++;
		builder->num_restarts++;
		builder->restarts[builder->restart_id] = builder->offset;
		builder->counter = 0;
		put_var32(builder, 0);
		put_var32(builder, key->size);
		put_var32(builder, value->size);
		append_buffer(builder, key->data, key->size);
		append_buffer(builder, value->data, value->size);
		set_last_key(builder->last_key, key->data, key->size);
		return 0;
	}
	const size_t non_shared = key->size - shared;
	put_var32(builder, shared);
	put_var32(builder, non_shared);
	put_var32(builder, value->size);
	append_buffer(builder, key->data + shared, non_shared);
	append_buffer(builder, value->data, value->size);
	set_last_key(builder->last_key, key->data, key->size);
	builder->counter++;
	return 0;
}

int writefile_append(char *file, char *data, size_t n)
{
	int fd = 0;
	if ((fd = open(file, O_RDWR)) == -1) {
		printf("open file fail %s\n", file);
		return -1;
	}
	lseek(fd, 0, SEEK_END);
	return write_raw(fd, data, n);
}

char *footer_encode_to(struct footer *foot)
{
	char *buf = malloc(foot->encode_length);
	size_t meta_index_offset = 0;
	size_t index_offset = 0;
	size_t offset = 0;
	offset = put_varint64(buf, foot->metaindex_handle->offset);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->metaindex_handle->size);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->index_handle->offset);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->index_handle->size);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	size_t pad_size = foot->encode_length - 8 -index_offset;
	memcpy(buf + index_offset, "0xff", pad_size);
	index_offset += pad_size;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_fixed32(buf + index_offset, (uint32_t)(kTableMagicNumber & 0xffffffffu));
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_fixed32(buf + index_offset, (uint32_t)(kTableMagicNumber >> 32));
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	assert(index_offset == 48);
	return buf; 
}
char *rocksdb_footer_encode_to(struct footer *foot)
{
	char *buf = malloc(foot->encode_length);
	size_t meta_index_offset = 0;
	size_t index_offset = 0;
	size_t offset = 0;
        char type = 1;
	memcpy(buf, &type, 1);
	index_offset +=1;
	offset = put_varint64(buf + index_offset, foot->metaindex_handle->offset);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->metaindex_handle->size);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->index_handle->offset);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_varint64(buf + index_offset, foot->index_handle->size);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	size_t pad_size = foot->encode_length - 12 -index_offset;
	memcpy(buf + index_offset, "0xff", pad_size);
	index_offset += pad_size;
	offset = put_fixed32(buf + index_offset, (uint32_t)1);
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_fixed32(buf + index_offset, (uint32_t)(rTableMagicNumber & 0xffffffffu));
	index_offset += offset;
	//	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	offset = put_fixed32(buf + index_offset, (uint32_t)(rTableMagicNumber >> 32));
	index_offset += offset;
	DEBUG("%d--%d--%d--%d\n", offset, foot->metaindex_handle->offset, index_offset, foot->index_handle->offset);
	assert(index_offset == 53);
	return buf; 
}
struct block_handle *new_block_handle()
{
	struct block_handle *handle = malloc(sizeof (struct block_handle));
	set_offset(handle, 0);
	set_size(handle, 0);
	handle->encode_max_length = kMaxEncodedLength;
	return handle;
} 
struct footer *new_footer()
{
	struct footer *foot = malloc(sizeof(struct footer));
	foot->metaindex_handle = new_block_handle();
	foot->index_handle = new_block_handle();
	foot->encode_length = kEncodedLength; 
	return foot;
}

struct properities_block *new_properities_block()
{
	struct properities_block *props= malloc(sizeof(struct properities_block));
	props->buffer = malloc(1024);
	props->user_collected_properties = malloc(100);
	props->offset = 0;
	props->c_offset = 0;
	memset(props->restarts, 0, 32 * sizeof(uint32_t));
	props->num_restarts = 1;
	props->restart_id = 0;
/*	struct rb_root *tree = (struct rb_root *)malloc(sizeof(struct rb_root));
	tree->rb_node = NULL;
	props->kvmap = tree;
*/
	return props;
}
 
struct meta_block *new_meta_block()
{
	struct meta_block *block= malloc(sizeof(struct meta_block));
	return block;
}

struct footer *new_rocksdb_footer()
{
	struct footer *foot = malloc(sizeof(struct footer));
	foot->metaindex_handle = new_block_handle();
	foot->index_handle = new_block_handle();
	foot->encode_length = 53; 
	return foot;
}
void block_reset(struct block_builder *rep){
	memset(rep->buffer, 0, BLOCK_SIZE);
	rep->offset = 0;
	rep->finished = false;	
	memset(rep->restarts, 0, 32 * sizeof(uint32_t));
  	rep->counter = 0;
 	rep->restart_id= 0;
	rep->num_restarts = 1;
}

struct slice *block_builder_finish(struct block_builder *block)
{
	data_block_add_restart(block);
	return new_slice(block->buffer, block->offset);
}
struct slice *index_block_finish(struct index_block *block)
{
	index_block_add_restart(block);
	return new_slice(block->buffer, block->offset);
}
struct slice *meta_index_block_finish(struct meta_index_block *block)
{

	size_t offset = put_fixed32(block->buffer + block->offset, 0);
	block->offset += offset;
	offset = put_fixed32(block->buffer + block->offset, 1);
	block->offset += offset;
	return new_slice(block->buffer, block->offset);
}

/*
void table_flush(struct table_rep *rep, )
{
	assert(!r->closed);
	if (!ok()) return;
	if (r->data_block.empty()) return;
	assert(!r->pending_index_entry);
	write_block(&r->data_block, &r->pending_handle);
	if (ok()) {
		r->pending_index_entry = true;
		r->status = r->file->Flush();
	}
}

*/
/*
static uint32_t BloomHash(const slice_t *key) {
	DEBUG("%s--%d\n", key->data, key->size);
	return Hash(key->data, key->size, 0xbc9f1d34);
}

struct filter_policy * new_policy()
{

}

char * create_filter(const slice_t *keys[], int n, char *buf, int bits_per_key_,int k_, int bufsize){
    size_t bits = n * bits_per_key_;
    if (bits < 64) bits = 64;
    size_t bytes = (bits + 7) / 8;
    bits = bytes * 8;
    //const size_t init_size = dst->size();
    const size_t init_size = bufsize;
    char *dst = malloc(init_size + bytes);
    memset(dst, 0, init_size + bytes);
   // memcpy(dst, buf, bufsize);
//    dst->resize(init_size + bytes, 0);
 //   dst->push_back(static_cast<char>(k_));
    //char* array = &(*dst)[init_size];
    char* array = &(dst)[init_size];
    int i;
    for (i = 0; i < n; i++) {
      uint32_t h = BloomHash(keys[i]);
      const uint32_t delta = (h >> 17) | (h << 15);
      size_t j;
      for (j = 0; j < k_; j++) {
        const uint32_t bitpos = h % bits;
        array[bitpos/8] |= (1 << (bitpos % 8));
        h += delta;
      }

    }
    return new_slice(dst, init_size + bytes);
}

bool key_may_match(const slice_t *key, const slice_t *bloom_filter){
    const size_t len = bloom_filter->size;
    if (len < 2) return false;
    const char* array = bloom_filter->data;
    const size_t bits = (len - 1) * 8;
    const size_t k = array[len-1]; 
    if (k > 30) {
       DEBUG("%d len=%d bits=%d\n", k, len, bits);
      return true;
    }
    uint32_t h = BloomHash(key);   
    const uint32_t delta = (h >> 17) | (h << 15);
    size_t j;
    for (j = 0; j < k; j++) { 
      const uint32_t bitpos = h % bits; 
      if ((array[bitpos/8] & (1 << (bitpos % 8))) == 0) return false;
      h += delta;
    }
    DEBUG("%d\n", k);
    return true;
}
*/
/*void add_properities_collectors(struct properities_block *props, char *key, int key_size, char *value, int value_size)
{
	memcpy(props->buffer + props->offset, key, key_size);
	props->offset += key_size;
	memcpy(props->buffer + props->offset, value, value_size);
	props->offset += value_size;

}
*/
void add_properities(struct properities_block *builder, char *key, int key_size, char *value, int value_size)
{
	int shared = 0;
	const size_t non_shared = key_size - shared;
	pput_var32(builder, shared);
	pput_var32(builder, non_shared);
	pput_var32(builder, value_size);
	memcpy(builder->buffer + builder->offset, key, key_size);
	builder->offset += key_size;
	memcpy(builder->buffer + builder->offset, value, value_size);
	builder->offset += value_size;
	
}
void add_properities_number(struct properities_block *builder, char *key, int key_size, uint64_t number)
{
	int shared = 0;
	const size_t non_shared = key_size - shared;
	pput_var32(builder, shared);
	pput_var32(builder, non_shared);
	int value_size = varint_length(number);
	pput_var32(builder, value_size);
	memcpy(builder->buffer + builder->offset, key, key_size);
	builder->offset += key_size;
	size_t offset = put_varint64(builder->buffer + builder->offset, number);
	builder->offset += offset;
	builder->restart_id++;
	builder->num_restarts++;
	builder->restarts[builder->restart_id] = builder->offset;
}
void add_properities_fix32_number(struct properities_block *builder, char *key, int key_size, uint32_t number)
{
	int shared = 0;
	const size_t non_shared = key_size - shared;
	pput_var32(builder, shared);
	pput_var32(builder, non_shared);
	int value_size = varint_length(number);
	pput_var32(builder, 4);
	memcpy(builder->buffer + builder->offset, key, key_size);
	builder->offset += key_size;
	size_t offset = put_fixed32(builder->buffer + builder->offset, number);
	builder->offset += offset;
	builder->restart_id++;
	builder->num_restarts++;
	builder->restarts[builder->restart_id] = builder->offset;
}
void add_properities_fix64_number(struct properities_block *builder, char *key, int key_size, uint64_t number)
{
	int shared = 0;
	const size_t non_shared = key_size - shared;
	pput_var32(builder, shared);
	pput_var32(builder, non_shared);
	int value_size = varint_length(number);
	pput_var32(builder, 8);
	memcpy(builder->buffer + builder->offset, key, key_size);
	builder->offset += key_size;
	size_t offset = put_fixed64(builder->buffer + builder->offset, number);
	builder->offset += offset;
	builder->restart_id++;
	builder->num_restarts++;
	builder->restarts[builder->restart_id] = builder->offset;
}

void add_properities_collector_names(struct properities_block *props, char *name, char *val)
{
/*	memcpy(props->user_collected_properties + props->c_offset, name, strlen(name));
	props->c_offset += strlen(name);
	memcpy(props->user_collected_properties + props->c_offset, val, strlen(val));
	props->c_offset += strlen(val);
*/
//	char *key2 = "zhang2";
//	put(props->kvmap, key2, key2);
}
void add_properities_collector_val32_number(struct properities_block * props, char *name, uint32_t val)
{
	memcpy(props->user_collected_properties + props->c_offset, name, strlen(name));
	props->c_offset += strlen(name);
	size_t offset = put_varint32(props->user_collected_properties + props->c_offset, val);
	props->c_offset += offset;
}

void add_properities_collector_fix32_number(struct properities_block * props, char *name, uint32_t val)
{
	memcpy(props->user_collected_properties + props->c_offset, name, strlen(name));
	props->c_offset += strlen(name);
	size_t offset = put_fixed32(props->user_collected_properties + props->c_offset, val);
	props->c_offset += offset;
}
void add_properities_collector_fix64_number(struct properities_block * props, char *name, uint32_t val)
{
	memcpy(props->user_collected_properties + props->c_offset, name, strlen(name));
	props->c_offset += strlen(name);
	size_t offset = put_fixed64(props->user_collected_properties + props->c_offset, val);
	props->c_offset += offset;
}
struct slice *properities_block_finish(struct properities_block *block)
{	
/*	add_properities_number(block,kIndexPartitions,strlen(kIndexPartitions),block->index_partitions);
	add_properities_number(block,kTopLevelIndexSize,strlen(kTopLevelIndexSize),block->top_level_index_size);
	add_properities_number(block,kIndexKeyIsUserKey,strlen(kIndexKeyIsUserKey),block->index_key_is_user_key);
	add_properities_number(block,kIndexValueIsDeltaEncoded,strlen(kIndexValueIsDeltaEncoded),block->index_value_is_delta_encoded);
	add_properities_number(block,kDeletedKeys,strlen(kDeletedKeys),block->num_deletions);
	add_properities_number(block,kMergeOperands,strlen(kMergeOperands),block->num_merge_operands);
	add_properities_number(block,kNumRangeDeletions,strlen(kNumRangeDeletions),block->num_range_deletions);
	add_properities_number(block,kFormatVersion,strlen(kFormatVersion),block->format_version);
	add_properities_number(block,kFixedKeyLen,strlen(kFixedKeyLen),block->fixed_key_len);
	add_properities_number(block,kCreationTime,strlen(kCreationTime),block->creation_time);
	add_properities_number(block,kOldestKeyTime,strlen(kOldestKeyTime),block->oldest_key_time);
	add_properities(block,kFilterPolicy,strlen(kFilterPolicy),block->filter_policy_name,strlen(block->filter_policy_name));
	add_properities(block,kComparator,strlen(kComparator),block->comparator_name,strlen(block->comparator_name));
	add_properities(block,kMergeOperator,strlen(kMergeOperator),block->merge_operator_name,strlen(block->merge_operator_name));
	add_properities(block,kPrefixExtractorName,strlen(kPrefixExtractorName),block->prefix_extractor_name,strlen(block->prefix_extractor_name));
	add_properities(block,kFilterPolicy,strlen(kFilterPolicy),block->filter_policy_name,strlen(block->filter_policy_name));
	add_properities(block,kPropertyCollectors,strlen(kPropertyCollectors),block->user_collected_properties,block->c_offset);
	add_properities_number(block,kRawValueSize,strlen(kRawValueSize),block->raw_value_size);
	add_properities_number(block,kFilterSize,strlen(kFilterSize),block->filter_size);
*/
	add_properities_number(block,kColumnFamilyId,strlen(kColumnFamilyId),block->column_family_id);
	add_properities(block,kColumnFamilyName,strlen(kColumnFamilyName),block->column_family_name,strlen(block->column_family_name));
	add_properities(block,kCompression,strlen(kCompression),block->compression_name,strlen(block->compression_name));
	add_properities_number(block,kDataSize,strlen(kDataSize),block->data_size);
	add_properities_fix64_number(block,kGlobalSeqno,strlen(kGlobalSeqno),0);
	add_properities_fix32_number(block,kVersion,strlen(kVersion),2);
	add_properities_number(block,kIndexSize,strlen(kIndexSize),block->index_size);
	add_properities_number(block,kNumDataBlocks,strlen(kNumDataBlocks),block->num_data_blocks);
	add_properities_number(block,kNumEntries,strlen(kNumEntries),block->num_entries);
	add_properities_number(block,kRawKeySize,strlen(kRawKeySize),block->raw_key_size);
	properity_block_add_restart(block);
	return new_slice(block->buffer, block->offset);
}

/*
void filter_start_block(uint64_t block_offset) {
  uint64_t filter_index = (block_offset / kFilterBase);
  assert(filter_index >= filter_offsets_.size());
  while (filter_index > filter_offsets_.size()) {
    GenerateFilter();
  }
}

Slice filter_block_finish() {
  if (!start_.empty()) {
    GenerateFilter();
  }
  const uint32_t array_offset = result_.size();
  for (size_t i = 0; i < filter_offsets_.size(); i++) {
    PutFixed32(&result_, filter_offsets_[i]);
  }
  PutFixed32(&result_, array_offset);
  result_.push_back(kFilterBaseLg);
  return Slice(result_);      
}
*/
