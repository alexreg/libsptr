#pragma once

#include <string.h>

#include "../sptr.h"

typedef void (* sptr_del_element_fn) (void * element);

struct sptr_array_metadata
{
	size_t element_size;
	sptr_del_element_fn del_element;
};

typedef void (* free_fn) (void * ptr);

#define inline_array(...) {__VA_ARGS__}

#define sptr_array(element_type, count, _init_value, _del_element) \
	({ \
		sptr_impl (sizeof (element_type) * count, _init_value, sptr_del_array, ((struct sptr_array_metadata) { sizeof (element_type), _del_element })); \
	}) \

#define sptr_array2(element_type, _array, _del_element) \
	({ \
		element_type array[] = _array; \
		size_t count = sizeof (array) / sizeof (element_type); \
		sptr_array (element_type, count, sptr_init_value_copy (array), _del_element); \
	}) \

void sptr_del_ptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_sptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_ptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);

void sptr_del_sptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size);
