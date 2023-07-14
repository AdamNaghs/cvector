# cvector

# DEFINE_VEC
 Creates a Vec type named {name}
 In Vec: data is the array, size is always 1 larger than the data in the
 array, capacity is the max amount of data we can hold before resizing ex: v
 has a size of 10, v.realloc(v,20); v still has a size of 10, but now a
 capacity of 20;

# ReturnItypes
 Creates a Return type named Return_{type}
 In Return: contains ptr to data, index of data, and error
 When using Return_{type}
 user must first ensure the err is VEC_OK before accessing the data or index
 if the err is VEC_OK then the data and index can be safely read.
 You can you unpack_type to do this for you.

 # Under the hood
 The user should not access the internal variables, and should
 only use the functions attached to the type via func ptr in the init.
 The methods used to define the func ptrs will be slightly mangled so they
 are different each time the macro is called,
 as long as the user doesnt use the same name twice.

 # Initialization
 User must define and pass a function to determine if their type is equal
 must return bool(technically an int) take arguements of (const * type, const * type)
 Comparisons for ints, floats, and strs(stdlib strcmp) are provided.
 You can call create_name to safely make a vector if you want to set it equal to a vector.
 ex: 
	 Vec_Int v = create_Vec_Int(compare_ints);
 as opposed to
	 Vec_Int v;
	 Vec_Int_init(v,compare_ints);

 The naming of the init is purposfully backward to because it is more of an internal method and slightly mangled.
 But if you want to manually an init errors then use the init.
  
 # Errors
 The error checking system ive made allows you to use to be able to forget able errors
 until they pop up.
 This is done through an unpacker for the Return_type.
 As well as the ASSERT_ON_ERROR and RETURN_ON_ERROR macros which will give some detailed error information. An error do these macros is a Vec_Error
 Also, whenever a function encounters an error it will run RETURN_ON_ERROR most of the time and give a detailed error to stderr
 and the program can continue to run. 
 You can either manually handle these errors or unpack Return_types and ASSERT_ON_ERROR.
 By unpacking and asserting on error

  # Vec_Error Codes
	  typedef enum
	{
		VEC_OK = 0,
		VEC_ALLOC_ERROR = 1,
		VEC_OOB_ERROR = 2,
		VEC_CANT_FIND_ERROR = 3,
		VEC_GIVEN_NULL_ERROR = 4
	} Vec_Error;

Behavior of Note: (all for functions called through func ptr except create/init)
 1. Freeing will leave the vec usable still as long as you call it though functions.
 2. Use free to empty the vector and set the size and capacity to 0.
 3. If you realloc/compact while the vec size is 0 the capacity will become 1.
 4. Clear removes all the data but leaves the capcity the same.
 5. You should not alter the size, capacity, or data, let the vec do it!
 6. When reading the code, if you find a function with a comment indicating it is internal, do not call it
 7. you can forget about most of the error handling and worry about it when it comes up
	by using unpack_##type, ASSERT_ON_ERROR, and RETURN_ON_ERROR
