#ifndef _VEC_H
#define _VEC_H /* 7/17/23 By Adam Naghavi*/

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

/*
To ensure modularity, the user can, and must for unique struct, define their own comparison
function to be used in the vector to ensure intended behavior.

Some common comparison functions are provided for the user but still must be
passed as an argument to the init to be used.
For strings use strcmp() from string.h
*/

/* these numbers are compatible with strcmp*/
typedef enum
{
	LESS = -1,
	EQUAL = 0,
	GREATER = 1,
} Comparison;

static Comparison compare_floats(const float *a, const float *b)
{
	/* Small number to account for floating point precision errors */
	const float epsilon = 0.00001;

	if (fabs(*a - *b) < epsilon)
		return EQUAL; /* Numbers are close enough to be equal */
	if (*a > *b)
		return GREATER;
	return LESS;
}

static Comparison compare_ints(const int *a, const int *b)
{
	if (*a == *b)
		return EQUAL;
	if (*a > *b)
		return GREATER;
	return LESS;
}

typedef enum
{
	VEC_OK = 0,
	VEC_ALLOC_ERROR = 1,
	VEC_OOB_ERROR = 2,
	VEC_CANT_FIND_ERROR = 3,
	VEC_GIVEN_NULL_ERROR = 4
} Vec_Error;

/*
 Creates a Vec type named {name}
 In Vec: data is the array, size is always 1 larger than the data in the
 array, capacity is the max amount of data we can hold before resizing ex: v
 has a size of 10, v.realloc(v,20); v still has a size of 10, but now a
 capacity of 20;

 Creates a Return type named Return_{type}
 In Return: contains ptr to data, index of data, and error
 When using Return_{type}
 user must first ensure the err is VEC_OK before accessing the data or index
 if the err is VEC_OK then the data and index can be safely read.

 The user should not access the internal variables, and should
 only use the functions attached to the type via func ptr in the init.
 The methods used to define the func ptrs will be slightly mangled so they
 are different each time the macro is called,
 as long as the user doesnt use the same name twice.

 User must define and pass a function to determine if their type is equal
 must return bool(technically an int) take arguements of (const * type, const * type)

Behavior of Note: (all for functions called through func ptr)
 1. Freeing will leave the vec usable still as long as you call it though functions.
 2. Use free to empty the vector and set the size and capacity to 0.
 3. If you realloc/compact while the vec size is 0 the capacity will become 1.
 4. Clear removes all the data but leaves the capcity the same.
 5. You should not alter the size, capacity, or data, let the vec do it!
 6. When reading the code, if you find a function with a comment indicating it is internal, do not call it
 7. you can forget about most of the error handling and worry about it when it comes up
by using unpack_##type, VEC_ASSERT_ON_ERROR, and VEC_RETURN_ON_ERROR
*/

#define VEC_ASSERT_ON_ERROR(err, func_name_string)                                                                                                                 \
	do                                                                                                                                                             \
	{                                                                                                                                                              \
		if (err != VEC_OK)                                                                                                                                         \
		{                                                                                                                                                          \
			fprintf(stderr, "Asserted on Error: in vector in function %s on line %d in file %s Vec_Error Code: %d \n", func_name_string, __LINE__, __FILE__, err); \
			assert(err == VEC_OK);                                                                                                                                 \
		}                                                                                                                                                          \
	} while (0)

#define VEC_RETURN_ON_ERROR(err, func_name_string)                                                                                                                  \
	do                                                                                                                                                              \
	{                                                                                                                                                               \
		if (err != VEC_OK)                                                                                                                                          \
		{                                                                                                                                                           \
			fprintf(stderr, "Returned on Error: in vector in function %s on line %d in file %s. Vec_Error Code: %d \n", func_name_string, __LINE__, __FILE__, err); \
			return err;                                                                                                                                             \
		}                                                                                                                                                           \
	} while (0)

#define VEC_RETURN_IF_NULL(ptr, func_name_string, err)                     \
	do                                                                     \
	{                                                                      \
		if (ptr == NULL)                                                   \
		{                                                                  \
			fprintf(stderr, "Error: NULL vector passed to %s function.\n", \
					func_name_string);                                     \
			return err;                                                    \
		}                                                                  \
	} while (0)

#define DEFINE_RETURN_TYPE(type)                                                \
	typedef struct Return_##type Return_##type;                                 \
                                                                                \
	struct Return_##type                                                        \
	{                                                                           \
		type *data;                                                             \
		size_t index;                                                           \
		Vec_Error err;                                                          \
	};                                                                          \
                                                                                \
	type *unpack_##type(Return_##type unpack)                                   \
	{                                                                           \
		VEC_ASSERT_ON_ERROR(unpack.err, "unpack_" #type);                       \
		return unpack.data;                                                     \
	}                                                                           \
	Return_##type internal_pack_##type(type *data, size_t index, Vec_Error err) \
	{                                                                           \
		Return_##type d = {data, index, err};                                   \
		return d;                                                               \
	}

#define VEC_INITIAL_CAPACITY 0

/*
	Functions:
	 bool (*empty)(this *);
	 size_t (*read_size)(this *);
	 size_t (*read_capacity)(this *);
	 Vec_Error (*free)(this *);
	 Vec_Error (*clear)(this *);
	 Vec_Error (*compact)(this *);
	 Vec_Error (*remove)(this *, size_t index);
	 Vec_Error (*realloc)(this *, size_t capacity);
	 Vec_Error (*push_back)(this *, type value);
	 Vec_Error (*insert)(this *, size_t index, type value);
	 Vec_Error (*swap)(this *, size_t index0, size_t index1);
	 Return_##type (*find)(this *, type value);
	 Return_##type (*at)(this *, size_t index);
	 void (*free_obj)(type value);
*/

/* define implementation */
#define DEFINE_VEC(type, name, compare_func)                                                                   \
	DEFINE_RETURN_TYPE(type);                                                                                  \
	typedef struct name name;                                                                                  \
	typedef Comparison (*Vec_Compare_Func_##type)(const type *, const type *);                                 \
                                                                                                               \
	struct name                                                                                                \
	{                                                                                                          \
		type *(data);                                                                                          \
		size_t(size);                                                                                          \
		size_t(capacity);                                                                                      \
		Vec_Compare_Func_##type(compare_vals);                                                                 \
		bool (*empty)(name *);                                                                                 \
		size_t (*read_size)(name *);                                                                           \
		size_t (*read_capacity)(name *);                                                                       \
		Vec_Error (*free)(name *);                                                                             \
		Vec_Error (*clear)(name *);                                                                            \
		Vec_Error (*compact)(name *);                                                                          \
		Vec_Error (*remove)(name *, size_t index);                                                             \
		Vec_Error (*realloc)(name *, size_t capacity);                                                         \
		Vec_Error (*push_back)(name *, type value);                                                            \
		Vec_Error (*insert)(name *, size_t index, type value);                                                 \
		Vec_Error (*swap)(name *, size_t index0, size_t index1);                                               \
		Return_##type (*find)(name *, type value);                                                             \
		Return_##type (*at)(name *, size_t index);                                                             \
		void (*free_obj)(type value);                                                                          \
	};                                                                                                         \
                                                                                                               \
	Vec_Error vec_realloc_##name(name *name##_v, size_t name##_capacity)                                       \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(name##_v, "realloc_" #type, VEC_GIVEN_NULL_ERROR);                                  \
		if (name##_capacity <= 0)                                                                              \
			name##_capacity = 1;                                                                               \
		type *new_data = (type *)realloc((name##_v->data), name##_capacity * sizeof(type));                    \
		VEC_RETURN_IF_NULL(new_data, "realloc_" #type, VEC_ALLOC_ERROR);                                       \
		(name##_v->data) = new_data;                                                                           \
		(name##_v->capacity) = name##_capacity;                                                                \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
	/*internal method used to determine if we need to resize and do it if we do*/                              \
	Vec_Error interal_try_resize_##name(name *v)                                                               \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "try_resize", VEC_GIVEN_NULL_ERROR);                                             \
		if ((v->size) == (v->capacity))                                                                        \
		{                                                                                                      \
			(v->capacity) = (v->capacity) == 0 ? 1 : (v->capacity) * 2;                                        \
			Vec_Error err = vec_realloc_##name(v, (v->capacity));                                              \
			VEC_RETURN_ON_ERROR(err, "internal_try_resize_" #name " (double)");                                \
			return VEC_OK;                                                                                     \
		}                                                                                                      \
		if ((v->size) < (v->capacity) / 4)                                                                     \
		{                                                                                                      \
			Vec_Error err = vec_realloc_##name(v, (v->capacity) / 2);                                          \
			VEC_RETURN_ON_ERROR(err, "internal_try_resize_" #name " (shrink)");                                \
			return VEC_OK;                                                                                     \
		}                                                                                                      \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
	/* internal method to check if index is oob */                                                             \
	Vec_Error internal_oob_check_##name(name *v, size_t index)                                                 \
	{                                                                                                          \
		if (index > (v->size))                                                                                 \
		{                                                                                                      \
			return VEC_OOB_ERROR;                                                                              \
		}                                                                                                      \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
	/* internal free obj method */                                                                             \
	void internal_free_obj_##name(name *v, type value)                                                         \
	{                                                                                                          \
		if (v->free_obj != NULL)                                                                               \
			(v->free_obj)(value);                                                                              \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_clear_##name(name *v)                                                                        \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "clear_" #name, VEC_GIVEN_NULL_ERROR);                                           \
		size_t i;                                                                                              \
		for (i = 0; i < (v->size); i++)                                                                        \
		{                                                                                                      \
			internal_free_obj_##name(v, ((v->data)[i]));                                                       \
			memset(&((v->data)[i]), 0, sizeof((v->data)[i]));                                                  \
		}                                                                                                      \
		(v->size) = 0;                                                                                         \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_remove_##name(name *v, size_t index)                                                         \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "remove_" #type, VEC_GIVEN_NULL_ERROR);                                          \
		VEC_RETURN_ON_ERROR(internal_oob_check_##name(v, index), "remove_" #type " (oob check index)_" #name); \
		internal_free_obj_##name(v, ((v->data)[index]));                                                       \
		size_t i;                                                                                              \
		for (i = index; i < ((v->size) - 1); i++)                                                              \
		{                                                                                                      \
			(v->data)[i] = (v->data)[i + 1];                                                                   \
		}                                                                                                      \
		(v->size)--;                                                                                           \
		Vec_Error err = interal_try_resize_##name(v);                                                          \
		VEC_RETURN_ON_ERROR(err, "remove_" #type "(internal_try_resize)_" #name);                              \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_free_##name(name *v)                                                                         \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "free_" #name, VEC_GIVEN_NULL_ERROR);                                            \
		if (v->free_obj != NULL)                                                                               \
		{                                                                                                      \
			size_t i;                                                                                          \
			for (i = 0; i < (v->size); i++)                                                                    \
			{                                                                                                  \
				internal_free_obj_##name(v, ((v->data)[i]));                                                   \
			}                                                                                                  \
		}                                                                                                      \
		free((v->data));                                                                                       \
		(v->data) = NULL;                                                                                      \
		(v->size) = 0;                                                                                         \
		(v->capacity) = 0;                                                                                     \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_swap_##name(name *v, size_t index0, size_t index1)                                           \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "swap", VEC_GIVEN_NULL_ERROR);                                                   \
		Vec_Error oob0 = internal_oob_check_##name(v, index0);                                                 \
		VEC_RETURN_ON_ERROR(oob0, "swap_" #type " (index0)");                                                  \
		Vec_Error oob1 = internal_oob_check_##name(v, index1);                                                 \
		VEC_RETURN_ON_ERROR(oob1, "swap_" #type " (index1)");                                                  \
		type tmp = (v->data)[index0];                                                                          \
		(v->data)[index0] = (v->data)[index1];                                                                 \
		(v->data)[index1] = tmp;                                                                               \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_compact_##name(name *v)                                                                      \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "compact_" #name, VEC_GIVEN_NULL_ERROR);                                         \
		if ((v->capacity) == (v->size))                                                                        \
			return VEC_OK;                                                                                     \
		Vec_Error err = vec_realloc_##name(v, (v->size));                                                      \
		VEC_RETURN_ON_ERROR(err, "compact");                                                                   \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	size_t vec_size_##name(name *v)                                                                            \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "size_" #name, SIZE_MAX);                                                        \
		return (v->size);                                                                                      \
	}                                                                                                          \
                                                                                                               \
	size_t vec_capacity_##name(name *v)                                                                        \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "capacity_" #name, SIZE_MAX);                                                    \
		return (v->capacity);                                                                                  \
	}                                                                                                          \
                                                                                                               \
	bool vec_empty_##name(name *v)                                                                             \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "empty_" #name, true);                                                           \
		return ((v->size) == 0);                                                                               \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_push_back_##name(name *v, type value)                                                        \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "push_back_" #name, VEC_GIVEN_NULL_ERROR);                                       \
		Vec_Error err = interal_try_resize_##name(v);                                                          \
		VEC_RETURN_ON_ERROR(err, "push_back_" #type " (interal_try_resize)_" #name);                           \
		(v->data)[(v->size)++] = value;                                                                        \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Vec_Error vec_insert_##name(name *v, size_t index, type value)                                             \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "insert_" #name, VEC_GIVEN_NULL_ERROR);                                          \
		VEC_RETURN_ON_ERROR(internal_oob_check_##name(v, index), "insert (oob check index)");                  \
		Vec_Error err = interal_try_resize_##name(v);                                                          \
		VEC_RETURN_ON_ERROR(err, "insert_" #type " (interal_try_resize)_" #name);                              \
		size_t i;                                                                                              \
		for (i = (v->size); i > index; i--)                                                                    \
		{                                                                                                      \
			(v->data)[i] = (v->data)[i - 1];                                                                   \
		}                                                                                                      \
		(v->data)[index] = value;                                                                              \
		(v->size)++;                                                                                           \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	Return_##type vec_find_##name(name *v, type value)                                                         \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "find_" #name,                                                                   \
						   (internal_pack_##type(NULL, -1, VEC_GIVEN_NULL_ERROR)));                            \
		int i;                                                                                                 \
		for (i = 0; i < (v->size); i++)                                                                        \
		{                                                                                                      \
			if ((compare_func)(&((v->data)[i]), &value) == EQUAL)                                              \
				return internal_pack_##type((&((v->data)[i])), (size_t)i, VEC_OK);                             \
		}                                                                                                      \
		return internal_pack_##type(NULL, -1, VEC_CANT_FIND_ERROR);                                            \
	}                                                                                                          \
                                                                                                               \
	Return_##type vec_at_##name(name *v, size_t index)                                                         \
	{                                                                                                          \
		VEC_RETURN_IF_NULL(v, "at_" #name,                                                                     \
						   (internal_pack_##type(NULL, -1, VEC_GIVEN_NULL_ERROR)));                            \
		Vec_Error err = internal_oob_check_##name(v, index);                                                   \
		if (err != VEC_OK)                                                                                     \
			return internal_pack_##type(NULL, -1, err);                                                        \
		return internal_pack_##type((&((v->data)[index])), index, VEC_OK);                                     \
	}                                                                                                          \
                                                                                                               \
	Vec_Error name##_init(name *v, void (*free_element)(type))                                                 \
	{                                                                                                          \
		CREATE_VEC(v, compare_func, type, name);                                                               \
		if (free_element != NULL)                                                                              \
			v->free_obj = free_element;                                                                        \
		VEC_RETURN_IF_NULL(v->data, "init_" #name, VEC_ALLOC_ERROR);                                           \
		return VEC_OK;                                                                                         \
	}                                                                                                          \
                                                                                                               \
	name create_##name(void (*free_element)(type))                                                             \
	{                                                                                                          \
		name init_vec;                                                                                         \
		Vec_Error init_err = name##_init(&init_vec, free_element);                                             \
		if (init_err != VEC_OK)                                                                                \
		{                                                                                                      \
			fprintf(stderr, "Vec Init Error: in vector on line %d in file %s. \
 Vec_Error Code: %d \n",                                                                                       \
					__LINE__, __FILE__, init_err);                                                             \
		}                                                                                                      \
		assert(init_err == VEC_OK);                                                                            \
		return init_vec;                                                                                       \
	}

/*
	Creates vector, indended use is for debugging with mem_debug.h
	Using this will allow for locating the initilization line and file.
*/
#define CREATE_VEC(v, compare_func, type, name)     \
	do                                              \
	{                                               \
		((v)->data) = (type *)malloc(sizeof(type)); \
		((v)->size) = 0;                            \
		((v)->capacity) = 1;                        \
		((v)->compare_vals) = compare_func;         \
		((v)->clear) = vec_clear_##name;            \
		((v)->free) = vec_free_##name;              \
		((v)->read_size) = vec_size_##name;         \
		((v)->read_capacity) = vec_capacity_##name; \
		((v)->empty) = vec_empty_##name;            \
		((v)->compact) = vec_compact_##name;        \
		((v)->realloc) = vec_realloc_##name;        \
		((v)->push_back) = vec_push_back_##name;    \
		((v)->remove) = vec_remove_##name;          \
		((v)->swap) = vec_swap_##name;              \
		((v)->find) = vec_find_##name;              \
		((v)->at) = vec_at_##name;                  \
		((v)->insert) = vec_insert_##name;          \
		((v)->free_obj) = NULL;                     \
	} while (0)

#endif /*_VEC_H*/