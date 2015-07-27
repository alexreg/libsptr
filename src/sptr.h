#pragma once

#include <stdbool.h>
#include <stdlib.h>

#define typeof(_type) __typeof (_type)

#define memory_barrier() __sync_synchronize ()

#define atomic_increment(_ptr) __sync_add_and_fetch (_ptr, 1)

#define atomic_decrement(_ptr) __sync_sub_and_fetch (_ptr, 1)

#define atomic_acquire(_ptr) __sync_lock_test_and_set (_ptr, true)

#define atomic_release(_ptr) __sync_lock_release (_ptr)

#define cleanup(_fn) __attribute__ ((cleanup (_fn)))

#define smart cleanup (sptr_cleanup)

#define noop ;

#define sptr_get_head_ptr(sptr_get_head_ptr_ptr) (* (struct sptr_head * *) ((void *) sptr_get_head_ptr_ptr - sizeof (struct sptr_head * *)))

#define sptr_init_assign(sptr_init_assign_value) \
	({ \
		* (typeof (sptr_init_assign_value) *) sptr_call_init_ptr_ptr = sptr_init_assign_value; \
	}) \

#define sptr_init_copy(sptr_init_copy_value) \
	({ \
		memcpy (sptr_call_init_ptr_ptr, sptr_init_copy_value, sizeof (sptr_init_copy_value)); \
	}) \

#define call_init_ptr(sptr_call_init_ptr__ptr, sptr_call_init_ptr_init) \
	({ \
		void * sptr_call_init_ptr_ptr = sptr_call_init_ptr__ptr; \
		sptr_call_init_ptr_init; \
	}) \

#define sptr_impl(sptr_impl__value_size, sptr_impl_init_value, sptr_impl__del_value, sptr_impl__metadata_size, sptr_impl_init_metadata) \
	({ \
		size_t sptr_impl_value_size = sptr_impl__value_size; \
		sptr_del_fn sptr_impl_del_value = sptr_impl__del_value; \
		size_t sptr_impl_metadata_size = sptr_impl__metadata_size; \
		\
		struct sptr_head * sptr_impl_head_ptr = malloc (sizeof (struct sptr_head) + sptr_impl_metadata_size + sizeof (struct sptr_head * *) + sptr_impl_value_size); \
		if (sptr_impl_head_ptr == NULL) \
			NULL; \
		sptr_impl_head_ptr->del_value = sptr_impl_del_value; \
		sptr_impl_head_ptr->free_lock = false; \
		sptr_impl_head_ptr->ref_count = 1; \
		sptr_impl_head_ptr->metadata_size = sptr_impl_metadata_size; \
		sptr_impl_head_ptr->value_size = sptr_impl_value_size; \
		\
		void * sptr_impl_metadata_ptr = (void *) sptr_impl_head_ptr + sizeof (struct sptr_head); \
		call_init_ptr (sptr_impl_metadata_ptr, sptr_impl_init_metadata); \
		*(struct sptr_head * *) ((void *) sptr_impl_metadata_ptr + sptr_impl_metadata_size) = sptr_impl_head_ptr; \
		void * sptr_impl_value_ptr = (void *) sptr_impl_metadata_ptr + sptr_impl_metadata_size + sizeof (struct sptr_head * *); \
		call_init_ptr (sptr_impl_value_ptr, sptr_impl_init_value); \
		memory_barrier (); \
		sptr_impl_value_ptr; \
	}) \

#define sptr(sptr__value_size, sptr_init_value, sptr__del_value, sptr_metadata) \
	({ \
		size_t sptr_value_size = sptr__value_size; \
		sptr_del_fn sptr_del_value = sptr__del_value; \
		\
		sptr_impl (sptr_value_size, sptr_init_value, sptr_del_value, sizeof (sptr_metadata), sptr_init_assign (sptr_metadata)); \
	}) \

#define sptr2(sptr2_value, sptr2__del_value, sptr2_metadata) \
	({ \
		sptr_del_fn sptr2_del_value = sptr2__del_value; \
		\
		sptr (sizeof (sptr2_value), sptr_init_assign (sptr2_value), sptr2_del_value, sptr2_metadata); \
	}) \

#define sptr3(sptr3__ptr_size, sptr3__ptr, sptr3__del_value, sptr3_metadata) \
	({ \
		size_t sptr3_ptr_size = sptr3__ptr_size; \
		void * sptr3_ptr = sptr3__ptr; \
		sptr_del_fn sptr3_del_value = sptr3__del_value; \
		\
		sptr (sptr3_ptr_size, sptr_init_copy (sptr3_ptr), sptr3_del_value, sptr3_metadata); \
	}) \

#define sptr_free(sptr_free__ptr) \
	({ \
		void * sptr_free_ptr = sptr_free__ptr; \
		\
		struct sptr_head * sptr_free_head_ptr = sptr_get_head_ptr (sptr_free_ptr); \
		if (!atomic_acquire (&sptr_free_head_ptr->free_lock)) \
		{ \
			void * sptr_free_metadata_ptr = (void *) sptr_free_head_ptr + sizeof (struct sptr_head); \
			if (sptr_free_head_ptr->del_value != NULL) \
				sptr_free_head_ptr->del_value (sptr_free_ptr, sptr_free_head_ptr->value_size, sptr_free_metadata_ptr, sptr_free_head_ptr->metadata_size); \
			free (sptr_free_head_ptr); \
		} \
	}) \

#define sptr_dup(sptr_dup__ptr) \
	({ \
		void * sptr_dup_ptr = sptr_dup__ptr; \
		\
		struct sptr_head * sptr_dup_head_ptr = sptr_get_head_ptr (sptr_dup_ptr); \
		void * sptr_dup_metadata_ptr = (void *) sptr_dup_head_ptr + sizeof (struct sptr_head); \
		void * sptr_dup_value_ptr = (void *) sptr_dup_metadata_ptr + sptr_dup_head_ptr->metadata_size + sizeof (struct sptr_head * *); \
		sptr_impl (sptr_dup_head_ptr->value_size, sptr_init_copy (sptr_dup_value_ptr), sptr_dup_head_ptr->del_value, sptr_dup_head_ptr->metadata_size, sptr_init_copy (sptr_dup_metadata_ptr)); \
	}) \

#define sptr_resize(sptr_resize__ptr, sptr_resize__new_value_size) \
	({ \
		void * sptr_resize_ptr = sptr_resize__ptr; \
		size_t sptr_resize_new_value_size = sptr_resize__new_value_size; \
		\
		struct sptr_head * sptr_resize_head_ptr = sptr_get_head_ptr (sptr_resize_ptr); \
		size_t sptr_resize_new_size = sizeof (struct sptr_head) + sptr_resize_head_ptr->metadata_size + sizeof (struct sptr_head * *) + sptr_resize_new_value_size; \
		if (realloc (sptr_resize_head_ptr, sptr_resize_new_value_size) == NULL) \
			false; \
		sptr_resize_head_ptr->value_size = sptr_resize_new_value_size; \
		true; \
	}) \

#define sptr_size(sptr_size__ptr) \
	({ \
		void * sptr_size_ptr = sptr_size__ptr; \
		\
		struct sptr_head * sptr_size_head_ptr = sptr_get_head_ptr (sptr_size_ptr); \
		sptr_size_head_ptr->value_size; \
	}) \

#define sptr_ref(sptr_ref__ptr) \
	({ \
		void * sptr_ref_ptr = sptr_ref__ptr; \
		\
		struct sptr_head * sptr_ref_head_ptr = sptr_get_head_ptr (sptr_ref_ptr); \
		sptr_ref_head_ptr->ref_count; \
	}) \

#define sptr_cpy(sptr_cpy__ptr) \
	({ \
		void * sptr_cpy_ptr = sptr_cpy__ptr; \
		\
		struct sptr_head * sptr_cpy_head_ptr = sptr_get_head_ptr (sptr_cpy_ptr); \
		atomic_increment (&sptr_cpy_head_ptr->ref_count); \
		memory_barrier (); \
		sptr_cpy_ptr; \
	}) \

#define sptr_rel(sptr_rel__ptr) \
	({ \
		void * sptr_rel_ptr = sptr_rel__ptr; \
		\
		struct sptr_head * sptr_rel_head_ptr = sptr_get_head_ptr (sptr_rel_ptr); \
		if (atomic_decrement (&sptr_rel_head_ptr->ref_count) == 0) \
			sptr_free (sptr_rel_ptr); \
		memory_barrier (); \
		sptr_rel_ptr; \
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
