#pragma once

#include <stdbool.h>
#include <stdlib.h>

#define typeof(type) __typeof (type)

#define auto_type __auto_type

#define memory_barrier() __sync_synchronize ()

#define atomic_increment(ptr) __sync_add_and_fetch (ptr, 1)

#define atomic_decrement(ptr) __sync_sub_and_fetch (ptr, 1)

#define atomic_acquire(ptr) __sync_lock_test_and_set (ptr, true)

#define atomic_release(ptr) __sync_lock_release (ptr)

#define cleanup_a(fn) __attribute__ ((cleanup (fn)))

#define smart cleanup_a (sptr_cleanup)

#define noop ;

#define get_head_ptr(ptr) (* (struct sptr_head * *) ((void *) ptr - sizeof (struct sptr_head * *)))

#define sptr_impl(_value_size, _init_value, _del_value, metadata) \
	({ \
		struct sptr_head * head_ptr = malloc (sizeof (struct sptr_head) + sizeof (metadata) + sizeof (struct sptr_head * *) + _value_size); \
		if (head_ptr == NULL) \
			NULL; \
		head_ptr->del_value = _del_value; \
		head_ptr->free_lock = false; \
		head_ptr->ref_count = 1; \
		head_ptr->metadata_size = sizeof (metadata); \
		head_ptr->value_size = _value_size; \
		\
		typeof (metadata) * metadata_ptr = (void *) head_ptr + sizeof (struct sptr_head); \
		*metadata_ptr = metadata; \
		*(struct sptr_head * *) ((void *) metadata_ptr + sizeof (metadata)) = head_ptr; \
		void * value_ptr = (void *) metadata_ptr + sizeof (metadata) + sizeof (struct sptr_head * *); \
		_init_value; \
		memory_barrier (); \
		value_ptr; \
	}) \

#define sptr_init_value_assign(value) \
	({ \
		* (typeof (value) *) value_ptr = value; \
	}) \

#define sptr_init_value_copy(value) \
	({ \
		memcpy (value_ptr, value, sizeof (value)); \
	}) \

#define sptr(value, _del_value, metadata) \
	({ \
		sptr_impl (sizeof (value), sptr_init_value_assign (value), _del_value, metadata); \
	}) \

#define sptr2(ptr_size, ptr, _del_value, metadata) \
	({ \
		sptr_impl (ptr_size, sptr_init_value_copy (ptr), _del_value, metadata); \
	}) \

#define sptr_dup(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		size_t ptr_size = sizeof (struct sptr_head) + head_ptr->metadata_size + sizeof (struct sptr_head * *) + head_ptr->value_size; \
		struct sptr_head * dup_head_ptr = malloc (ptr_size); \
		if (dup_head_ptr == NULL) \
			NULL; \
		memcpy (dup_head_ptr, head_ptr, ptr_size); \
		memory_barrier (); \
		dup_head_ptr; \
	}) \

#define sptr_resize(ptr, new_value_size) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		size_t new_size = sizeof (struct sptr_head) + head_ptr->metadata_size + sizeof (struct sptr_head * *) + new_value_size; \
		if (realloc (head_ptr, new_size) == NULL) \
			false; \
		head_ptr->value_size = new_value_size; \
		true; \
	}) \

#define sptr_free(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		if (!atomic_acquire (&head_ptr->free_lock)) \
		{ \
			void * metadata_ptr = (void *) head_ptr + sizeof (struct sptr_head); \
			if (head_ptr->del_value != NULL) \
				head_ptr->del_value (ptr, head_ptr->value_size, metadata_ptr, head_ptr->metadata_size); \
			free (head_ptr); \
		} \
	}) \

#define sptr_size(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		head_ptr->value_size; \
	}) \

#define sptr_ref(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		head_ptr->ref_count; \
	}) \

#define sptr_cpy(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		atomic_increment (&head_ptr->ref_count); \
	}) \

#define sptr_rel(ptr) \
	({ \
		struct sptr_head * head_ptr = get_head_ptr (ptr); \
		if (atomic_decrement (&head_ptr->ref_count) == 0) \
			sptr_free (ptr); \
	}) \

typedef void (* sptr_del_fn) (void * value, size_t value_size, void * metadata, size_t metadata_size);

struct sptr_head
{
	sptr_del_fn del_value;
	
	bool free_lock;
	unsigned int ref_count;
	
	size_t metadata_size;
	size_t value_size;
};

static void sptr_cleanup (void * _ptr_ptr)
{
	void * * ptr_ptr = _ptr_ptr;
	if (ptr_ptr != NULL)
	{
		sptr_rel (*ptr_ptr);
		*ptr_ptr = NULL;
	}
}
