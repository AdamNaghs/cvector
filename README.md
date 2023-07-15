# cvector

# DEFINE_VEC(type,name,compare_func)
 Creates a Vec type named {name}
 
 In Vec: data is the array, size is always 1 larger than the data in the
 array, capacity is the max amount of data we can hold before resizing ex: v
 has a size of 10, v.realloc(v,20); v still has a size of 10, but now a
 capacity of 20;

# Return_types
 Creates a Return type named Return_{type}
 In Return: contains ptr to data, index of data, and error
 When using Return_{type}
 user must first ensure the err is VEC_OK before accessing the data or index
 if the err is VEC_OK then the data and index can be safely read.
 You can you unpack_type to do this for you.
 ex of Return_types:
 
 	/* pop from vector v where we find 5 (first occurance) */
  	Return_int ret = v->find(v,5);
 	int* i = unpack_int(ret); 
	Vec_Error err = v->remove(v,ret.index); 
	ASSERT_ON_ERR(err);
If we unpack successfully then we can access all of ret's data, otherwise we the program will crash and tell you the error.
After unpacking there is no reason for there to be an error but to be sure ASSERT_ON_ERR to ensure you end the program,
get an error and follow the error tree in stderr.
	
    	
   	

 # Under the hood
 The user should not access the internal functions (i.e. funcs with internal in the name), and should
 only use the functions attached to the type via func ptr in the init. Please reframe from reading the data directly or the size or capacity.
 The methods used to define the func ptrs will be slightly mangled so they
 are different each time the macro is called,
 as long as the user doesnt use the same name twice.

 # Initialization
 User must define and pass a function to determine if their type is equal
 must return bool(technically an int) take arguements of (const * type, const * type)
 Comparisons for ints, floats, and strs(stdlib strcmp) are provided.
 You can call create_name to safely make a vector if you want to set it equal to a vector.
 ex: 
 
	 Vec_Int v = create_Vec_Int();
 as opposed to
 
	 Vec_Int v;
	 Vec_Int_init(v);
The naming of the init is purposfully backward to ensure differentiation between create and init.

If you're debugging memory use CREATE_VEC to ensure __LINE__ & __FILE__ 
macros work for capturing location of initialization.
Otherwise you'll be told your Vec was allocated wherever you defined it with DEFINE_VEC.

	Vec_Int v ;
 	CREATE_VEC(&v,compare_ints,int,Vec_Int);

 
  
 # Errors & Unpacking
 The error checking system ive made allows you to use to be able to forget able errors
 until they pop up.
 This is done through an unpacker for the Return_type.
 As well as the ASSERT_ON_ERROR and RETURN_ON_ERROR macros which will give some detailed error information. An error do these macros is a Vec_Error
 Also, whenever a function encounters an error it will run RETURN_ON_ERROR most of the time and give a detailed error to stderr
 and the program can continue to run. 
 
 You can either manually handle these errors or unpack Return_types and ASSERT_ON_ERROR.
 By unpacking which will assert on error.
 
 Functions that return a size return SIZE_MAX on error.

  # Vec_Error Codes
	  typedef enum
	{
		VEC_OK = 0,
		VEC_ALLOC_ERROR = 1,
		VEC_OOB_ERROR = 2,
		VEC_CANT_FIND_ERROR = 3,
		VEC_GIVEN_NULL_ERROR = 4
	} Vec_Error;

 # Comparisons
 There is no way for me to know if some unique data type is equal so I leave it to a function pointer which reaturns
 a Comparison or -1 for LESS, 0 for EQUAL, 1 for GREATER
 
	 typedef enum
	{
		LESS = -1,
		EQUAL = 0,
		GREATER = 1,
	} Comparison;
I've written compare_ints and compare_floats functions and in the stdlib you can use strcmp to compare strings.
Any other types need their own compare method to be written and passed as an argument upon initialization.
The only method which uses the comparison func is find and it only uses the EQUAL.
You can use the comparison func to ensure you can use sorting algos on the struct.

# Functions
This struct simulates a class using function pointers but you still need to pass self back into the function.
You can forego this fascade and just call the mangled functions that are defined by DEFINE_VEC

	DEFINE_VEC(int,Vec_Int,compare_ints);
	Vec_Int v = create_Vec_Int();
	
	Return_int x = vec_at_Vec_Int(&v,1);
	int* x_val = unpack_int(x); 
As opposed to

	Return_int x = v.at(&v,1);
	int* x_val = unpack_int(x); 


# Behavior of Note: 
 1. Freeing will leave the vec usable still as long as you call it though functions.
 2. Use free to empty the vector and set the size and capacity to 0.
 3. If you realloc/compact while the vec size is 0 the capacity will become 1.
 4. Clear removes all the data but leaves the capcity the same.
 5. You should not alter the size, capacity, or data, let the vec do it!
 6. When reading the code, if you find a function with a comment indicating it is internal, do not call it
 7. you can forget about most of the error handling and worry about it when it comes up
	by using unpack_##type, ASSERT_ON_ERROR, and RETURN_ON_ERROR
 8. if youre using the memory debugger and a vec, change you init method to the CREATE_VEC macro,
	this macro takes many args but ensures the __LINE__ & __FILE__ macros work as expected.
 9. if you prefer not to use the object oriented way of using this struct you can call the function it defines
	and links as function ptrs. However the object oriented method provides the ability to easily change the type of your Vec
