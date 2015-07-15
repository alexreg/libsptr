#include <assert.h>
#include <stdlib.h>

#include "../sptr.h"

#include "util.h"

#define sptr_del_ptr_array_impl(ptr, value_size, metadata, metadata_size, free_element) \
	{ \
		assert (metadata_size = sizeof (size_t)); \
		size_t element_size = * (size_t *) metadata; \
		for (void * el = ptr; el < ptr + value_size; el += element_size) \
			free_element (el); \
	} \

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
