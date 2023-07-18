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
 
	/* Pass NULL If you dont need a function to free your data */
	 Vec_Int v = create_Vec_Int(NULL); 
 as opposed to
 
	 Vec_Int v;
	 Vec_Int_init(&v,NULL);
The naming of the init is purposfully backward to ensure differentiation between create and init.

If you're debugging memory use CREATE_VEC to ensure __LINE__ & __FILE__ 
macros work for capturing location of initialization.
Otherwise you'll be told your Vec was allocated wherever you defined it with DEFINE_VEC.

	Vec_Int v ;
 	CREATE_VEC(&v,compare_ints,int,Vec_Int);
	/* If you wanted to specify a free_func this way you must assign the variable */
	v.free_obj = func;

 
  
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
 There is no way for me to know if some unique data type is equal so I leave it to a function pointer which returns
 a Comparison or -1 for LESS, 0 for EQUAL, 1 for GREATER
 
	 typedef enum
	{
		LESS = -1,
		EQUAL = 0,
		GREATER = 1,
	} Comparison;
	/* 
		input of compare func must be: 
		(const type *, const type *)  
	*/

I've written compare_ints and compare_floats functions and in the stdlib you can use strcmp to compare strings.
Any other types need their own compare method to be written and passed as an argument upon initialization.
The only method which uses the comparison func is find and it only uses the EQUAL.
You can use the comparison func to ensure you can use sorting algos on the struct.

# Freeing
If you have data that needs a function to be ran on it before freeing you can specify a function to free with.
The Vec has a func ptr named free_obj you can assign to free your object.

After freeing a Vec it is ready to be used again.
Freeing frees all dynamic memory and sets the size and capacity to 0.
Clearing leaves the capacity but reduces the size to 0.

Example using threads:

    #include <windows.h>
	#include <stdio.h>

	#define NUM_THREADS 5

	Comparison MyCompareObjectHandles(HANDLE const *hFirst, HANDLE const *hSecond)
	{
    	return (hFirst == hSecond) ? EQUAL : LESS;
	}
	DEFINE_VEC(HANDLE, Threads, MyCompareObjectHandles);
	int main()
	{
		Threads vec = create_Threads((void(*)(void*))CloseHandle); /* cast to avoid warning */
		/* Create the threads */
		for (int i = 0; i < NUM_THREADS; i++)
		{
			vec.push_back(&vec, CreateThread(NULL, 0, allocateMemory, NULL, 0, NULL));
		}
		/* Wait for all threads to finish */
		for (int i = 0; i < NUM_THREADS; i++)
		{
			WaitForSingleObject(*unpack_HANDLE(vec.at(&vec, i)), INFINITE);
		}
		/* Clean up */
		vec.free(&vec); /* Calls CloseHandle on all threads and frees vec */
	    return 0;
	}


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
Here is a list of the member functions in the struct:

	Function Pointers:                                                                                            
	{                                                                                                        
		void (*free_obj)(type value);                                                                      
		Vec_Compare_Func_##type(compare_vals);                                                             
		bool (*empty)(name * this);                                                                             
		size_t (*read_size)(name * this);                                                                       
		size_t (*read_capacity)(name * this);                                                                   
		Vec_Error (*free)(name * this);                                                                         
		Vec_Error (*clear)(name * this);                                                                        
		Vec_Error (*compact)(name * this);                                                                      
		Vec_Error (*remove)(name * this, size_t index);                                                         
		Vec_Error (*realloc)(name * this, size_t capacity);                                                     
		Vec_Error (*push_back)(name * this, type value);                                                        
		Vec_Error (*insert)(name * this, size_t index, type value);                                             
		Vec_Error (*swap)(name * this, size_t index0, size_t index1);                                           
		Return_##type (*find)(name * this, type value);                                                         
		Return_##type (*at)(name * this, size_t index);                                                         
	}; 


# Behavior of Note: 
 1. Freeing will leave the vec usable still as long as you call it though functions.
 2. Use free to empty the vector and set the size and capacity to 0.
 3. If you realloc/compact while the vec size is 0 the capacity will become 1.
 4. Clear removes all the data but leaves the capcity the same.
 5. You should not alter the size, capacity, or data, let the vec do it!
 6. When reading the code, if you find a function with a comment indicating it is internal, do not call it
 7. You can forget about most of the error handling and worry about it when it comes up
	by using unpack_##type, ASSERT_ON_ERROR, and RETURN_ON_ERROR
 8. If youre using the memory debugger and a vec, change your init method to the CREATE_VEC macro,
	this macro takes many args but ensures the __LINE__ & __FILE__ macros work as expected.
 9. If you prefer not to use the object oriented way of using this struct you can call the function it defines
	and links as function ptrs. However the object oriented method provides the ability to easily change the type of your Vec
 10. Vec is not inherantly thread safe, that alone would have doubled the code base to control versioning. You must use your own mutexs or semaphores around each function call.

# Type specific implementation
If you want to alter the Vec's implementation for a specific type: 
expand the macro, use a formatter, and then alter whatever aspects are causing trouble
for your specific use case. 

The only time you would need to do this is if you want to free your structs using a specific command.
Ex: closing windows thread handles needs CloseHandle(threads[i]).

This can be avoided by writing a function to do your specific clearing/removing.


# Why did you make this? C++ already has a vector!
I wanted to stick to C and try to challenge myself to create a general vector.
Arrays are great but often you do not know how much data you need to store and its annoying rewritting a dynamic array for every data type I need. So writting a macro to do it all for me seemed like a no brainer. This was my first time using a macro to create an object for me so if you have any advice on how I did please let me know I would be interested to hear.

I decided to keep the complexity low and leave functions like pop out because you can recreate the functionality with two calls.
After testing the vector I decided a fun application would be a memory debugger. That debugger is included and allows the user to inspect their memory and detect and patch memory leaks.

If I had to rewrite this I'd probably lose the function pointers and shorten the names for the functions. I realize thats an easy change, but when defining the vector, while I did my best not to call anything through function pointer, there are a few places where I needed to use them. This could be remedied by increasing the number of arguments you need to pass to DEFINE_VEC, but you have to pass so many already I don't its worth the change. Also shortening the function names would be a deep refactor of all of this git repo which would be too much to ensure functionality. 