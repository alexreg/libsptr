#include <assert.h>
#include <stdlib.h>

#include "../sptr.h"

#include "util.h"

#define sptr_del_ptr_array_impl(sptr_del_ptr_array__ptr, sptr_del_ptr_array__value_size, sptr_del_ptr_array__metadata, sptr_del_ptr_array__metadata_size, sptr_del_ptr_array_free_element) \
	({ \
		void * sptr_del_ptr_array_ptr = sptr_del_ptr_array__ptr; \
		size_t sptr_del_ptr_array_value_size = sptr_del_ptr_array__value_size; \
		size_t * sptr_del_ptr_array_metadata = sptr_del_ptr_array__metadata; \
		size_t sptr_del_ptr_array_metadata_size = sptr_del_ptr_array__metadata_size; \
		\
		assert (sptr_del_ptr_array_metadata_size = sizeof (size_t)); \
		size_t sptr_del_ptr_array_element_size = * sptr_del_ptr_array_metadata; \
		for (void * sptr_del_ptr_array_el = sptr_del_ptr_array_ptr; sptr_del_ptr_array_el < sptr_del_ptr_array_ptr + sptr_del_ptr_array_value_size; sptr_del_ptr_array_el += sptr_del_ptr_array_element_size) \
			sptr_del_ptr_array_free_element (sptr_del_ptr_array_el); \
	}) \

void sptr_del_ptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size)
{
	void * * ptr_ptr = (void * *) ptr;
	free (*ptr_ptr);
}

void sptr_del_sptr (void * ptr, size_t value_size, void * metadata, size_t metadata_size)
{
	void * * ptr_ptr = (void * *) ptr;
	sptr_free (*ptr_ptr);
}

void sptr_del_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size)
{
	assert (metadata_size = sizeof (struct sptr_array_metadata));
	struct sptr_array_metadata * array_metadata = metadata;
	if (array_metadata->del_element != NULL)
	{
		for (void * el = ptr; el < ptr + value_size; el += array_metadata->element_size)
			array_metadata->del_element (el);
	}
}

void sptr_del_ptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size)
{
	sptr_del_ptr_array_impl (ptr, value_size, metadata, metadata_size, sptr_free);
}

void sptr_del_sptr_array (void * ptr, size_t value_size, void * metadata, size_t metadata_size)
{
	sptr_del_ptr_array_impl (ptr, value_size, metadata, metadata_size, sptr_free);
}
