#pragma once

#include <string.h>

#include "../sptr.h"

typedef void (* sptr_del_element_fn) (void * element);

struct sptr_array_metadata
{
	size_t element_size;
	sptr_del_element_fn del_element;
};

#define inline_array(...) {__VA_ARGS__}

#define sptr_array(sptr_array_element_type, sptr_array__count, sptr_array_init_value, sptr_array__del_element) \
	({ \
		size_t sptr_array_count = sptr_array__count; \
		sptr_del_element_fn sptr_array_del_element = sptr_array__del_element; \
		\
		sptr (sizeof (sptr_array_element_type) * sptr_array_count, sptr_array_init_value, &sptr_del_array, ((struct sptr_array_metadata) { sizeof (sptr_array_element_type), sptr_array_del_element })); \
	}) \

#define sptr_array2(sptr_array2_element_type, sptr_array2__array, sptr_array2__del_element) \
	({ \
		sptr_del_element_fn sptr_array2_del_element = sptr_array2__del_element; \
		\
		sptr_array2_element_type sptr_array2_array[] = sptr_array2__array; \
		size_t sptr_array2_count = sizeof (sptr_array2_array) / sizeof (sptr_array2_element_type); \
		sptr_array (sptr_array2_element_type, sptr_array2_count, sptr_init_copy (sptr_array2_array), sptr_array2_del_element); \
	}) \

#define sptr_array_resize(sptr_array_resize__ptr, sptr_array_resize__new_count) \
	({ \
		void * sptr_array_resize_ptr = sptr_array_resize__ptr; \
		size_t sptr_array_resize_new_count == sptr_array_resize__new_count; \
		\
		struct sptr_head * sptr_array_resize_head_ptr = sptr_get_head_ptr (sptr_array_resize_ptr); \
		struct sptr_array_metadata * sptr_array_resize_metadata = sptr_array_resize_head_ptr->metadata; \
		sptr_resize (sptr_array_resize_ptr, sptr_array_resize_metadata->element_size * sptr_array_resize_new_count); \
	}) \

#define sptr_array_size(sptr_array_size__ptr) \
	({ \
		void * sptr_array_size_ptr = sptr_array_size__ptr; \
		\
		struct sptr_head * sptr_array_size_head_ptr = sptr_get_head_ptr (sptr_array_size_ptr); \
		struct sptr_array_metadata * sptr_array_size_metadata = sptr_array_size_head_ptr->metadata; \
		sptr_size (sptr_array_size_ptr) / sptr_array_size_metadata->element_size; \
	}) \

void sptr_del_ptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_sptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_ptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_sptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);
