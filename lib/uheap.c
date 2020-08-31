
#include <inc/lib.h>
// malloc()
//	This function use BEST FIT strategy to allocate space in heap
//  with the given size and return void pointer to the start of the allocated space

//	To do this, we need to switch to the kernel, allocate the required space
//	in Page File then switch back to the user again.
//
//	We can use sys_allocateMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls allocateMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the allocateMem function is empty, make sure to implement it.


//==================================================================================//
//============================ REQUIRED FUNCTIONS ==================================//
//==================================================================================//
struct User_Heap
{
	uint32 size;
	uint32 address;
	int free;
	char name [20];
};

uint32 MAX_Count = 0;
struct User_Heap user_heap_info [200000];
int first = 1;
uint32 free_frames_user = 0; // if free_frames 0 there is no kfree , if free_frames 1 there is kfree

void* malloc(uint32 size)
{
	unsigned int frames_needed = size / PAGE_SIZE;
	uint32 * ptr_page_table = NULL;
	uint32 returned_value;
	if (size % PAGE_SIZE > 0)
	{
		frames_needed++;
	}
	// to initialize data in struct
	if (first == 1)
	{
		int j = 0;
		for(uint32 i = USER_HEAP_START;i<USER_HEAP_MAX;i+=PAGE_SIZE)
		{
			user_heap_info[j].address = i;
			user_heap_info[j].size = 0;
			user_heap_info[j].free = 1;
			j++;
		}
		MAX_Count = j;
		first = 0;
	}

	if (sys_isUHeapPlacementStrategyBESTFIT()){
		if (free_frames_user == 0)
		{
			uint32 start_address = 0;
			uint32 counter = 0;
			int index = 0;
			for(uint32 i = 0;i<MAX_Count;i++)
			{
				if (user_heap_info[i].free == 1)
				{
					if (counter == frames_needed)
					{
						start_address = user_heap_info[i-counter].address;
						index = i-counter;
						break;
					}
					counter++;
				}
				else
				{
					if (counter == frames_needed)
					{
						start_address = user_heap_info[i-counter].address;
						index = i-counter;
						break;
					}
				}
			}
			if (frames_needed > counter)
			{
				return NULL;
			}
			for(int i=0;i<frames_needed;i++)
			{
				user_heap_info[index].free = 0;
				user_heap_info[index].size = size;
				index++;
			}
			sys_allocateMem(start_address,size);
			return (void*) start_address;
		}

		else
		{
			uint32 start_ptr = 0;
			uint32 counter = 0;
			uint32 min_counter = 1000000000;
			int index = 0;
			for(uint32 i = 0;i<MAX_Count;i++)
			{
				if (i+1 == MAX_Count && counter != 0)
				{
					if (counter >= frames_needed)
					{
						if (min_counter > counter)
						{
							start_ptr = user_heap_info[i-counter].address;
							min_counter = counter;
							index = i-counter;
						}
					}
				}
				if (user_heap_info[i].free == 1)
				{
					counter++;
				}
				else
				{
					if (counter >= frames_needed)
					{
						if (min_counter > counter)
						{
							start_ptr = user_heap_info[i-counter].address;
							min_counter = counter;
							index = i-counter;
						}
					}
					counter = 0;
				}
			}
			if (frames_needed > counter)
				return NULL;

			for(int i = 0;i<frames_needed;i++)
			{
				user_heap_info[index].free = 0;
				user_heap_info[index].size = size;
				index++;
			}
			sys_allocateMem(start_ptr,size);
			return (void*) start_ptr;
		}
	}
	// Steps:
	//	1) Implement BEST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_allocateMem to invoke the Kernel for allocation
	// 	4) Return pointer containing the virtual address of allocated space,
	//

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyBESTFIT() to check the current strategy

	//change this "return" according to your answer
return NULL;
}

uint32 count_shared = 0;
int first_shared = 0;
void* smalloc(char *sharedVarName, uint32 size, uint8 isWritable)
{
		if(count_shared >= 97)
			return NULL;
		for(int i =0;i<count_shared;i++)
		{
			if (strcmp(user_heap_info[i].name,sharedVarName) == 0)
				return NULL;
		}
		unsigned int frames_needed = size / PAGE_SIZE;
		uint32 * ptr_page_table = NULL;
		uint32 returned_value;
		if (size % PAGE_SIZE > 0)
		{
			frames_needed++;
		}
		// to initialize data in struct
		if (first == 1)
		{
			int j = 0;
			for(uint32 i = USER_HEAP_START;i<USER_HEAP_MAX;i+=PAGE_SIZE)
			{
				user_heap_info[j].address = i;
				user_heap_info[j].size = 0;
				user_heap_info[j].free = 1;
				j++;
			}
			MAX_Count = j;
			first = 0;
		}

		if (sys_isUHeapPlacementStrategyBESTFIT()){
			if (first_shared == 0)
			{
				uint32 start_address = 0;
				uint32 counter = 0;
				int index = 0;
				for(uint32 i = 0;i<MAX_Count;i++)
				{
					if (user_heap_info[i].free == 1)
					{
						if (counter == frames_needed)
						{
							start_address = user_heap_info[i-counter].address;
							index = i-counter;
							break;
						}
						counter++;
					}
					else
					{
						if (counter == frames_needed)
						{
							start_address = user_heap_info[i-counter].address;
							index = i-counter;
							break;
						}
					}
				}
				if (frames_needed > counter)
				{
					return NULL;
				}
				for(int i=0;i<frames_needed;i++)
				{
					user_heap_info[index].free = 0;
					user_heap_info[index].size = size;
					strcpy(user_heap_info[index].name,sharedVarName);
					index++;
				}
				sys_createSharedObject(sharedVarName,size,isWritable,(void*)start_address);
				count_shared++;
				return (void*) start_address;
			}

			else
			{
				uint32 start_ptr = 0;
				uint32 counter = 0;
				uint32 min_counter = 1000000000;
				int index = 0;
				for(uint32 i = 0;i<MAX_Count;i++)
				{
					if (i+1 == MAX_Count && counter != 0)
					{
						if (counter >= frames_needed)
						{
							if (min_counter > counter)
							{
								start_ptr = user_heap_info[i-counter].address;
								min_counter = counter;
								index = i-counter;
							}
						}
					}
					if (user_heap_info[i].free == 1)
					{
						counter++;
					}
					else
					{
						if (counter >= frames_needed)
						{
							if (min_counter > counter)
							{
								start_ptr = user_heap_info[i-counter].address;
								min_counter = counter;
								index = i-counter;
							}
						}
						counter = 0;
					}
				}
				if (frames_needed > counter)
					return NULL;

				for(int i = 0;i<frames_needed;i++)
				{
					user_heap_info[index].free = 0;
					user_heap_info[index].size = size;
					strcpy(user_heap_info[index].name,sharedVarName);
					index++;
				}
				sys_createSharedObject(sharedVarName,size,isWritable,(void*)start_ptr);
				count_shared++;
				return (void*) start_ptr;
			}
		}
	//TODO: [PROJECT 2019 - MS2 - [6] Shared Variables: Creation] smalloc() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("smalloc() is not implemented yet...!!");
	// Steps:
	//	1) Implement BEST FIT strategy to search the heap for suitable space
	//		to the required allocation size (space should be on 4 KB BOUNDARY)
	//	2) if no suitable space found, return NULL
	//	 Else,
	//	3) Call sys_createSharedObject(...) to invoke the Kernel for allocation of shared variable
	//		sys_createSharedObject(): if succeed, it returns the ID of the created variable. Else, it returns -ve
	//	4) If the Kernel successfully creates the shared variable, return its virtual address
	//	   Else, return NULL

	//This function should find the space of the required range
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyBESTFIT() to check the current strategy

	//change this "return" according to your answer
	return 0;
}

void* sget(int32 ownerEnvID, char *sharedVarName)
{
	//TODO: [PROJECT 2019 - MS2 - [6] Shared Variables: Get] sget() [User Side]
	// Write your code here, remove the panic and write your code
	//panic("sget() is not implemented yet...!!");

	// Steps:
	//	1) Get the size of the shared variable (use sys_getSizeOfSharedObject())
	//	2) If not exists, return NULL
	//	3) Implement BEST FIT strategy to search the heap for suitable space
	//		to share the variable (should be on 4 KB BOUNDARY)
	//	4) if no suitable space found, return NULL
	//	 Else,
	//	5) Call sys_getSharedObject(...) to invoke the Kernel for sharing this variable
	//		sys_getSharedObject(): if succeed, it returns the ID of the shared variable. Else, it returns -ve
	//	6) If the Kernel successfully share the variable, return its virtual address
	//	   Else, return NULL
	//
	int size = sys_getSizeOfSharedObject(ownerEnvID,sharedVarName);
	if (size == E_SHARED_MEM_NOT_EXISTS)
		return NULL;
	else
	{
		unsigned int frames_needed = size / PAGE_SIZE;
		uint32 * ptr_page_table = NULL;
		uint32 returned_value;
		if (size % PAGE_SIZE > 0)
		{
			frames_needed++;
		}
		// to initialize data in struct
		if (first == 1)
		{
			int j = 0;
			for(uint32 i = USER_HEAP_START;i<USER_HEAP_MAX;i+=PAGE_SIZE)
			{
				user_heap_info[j].address = i;
				user_heap_info[j].size = 0;
				user_heap_info[j].free = 1;
				j++;
			}
			MAX_Count = j;
			first = 0;
		}

		if (sys_isUHeapPlacementStrategyBESTFIT()){
			if (first_shared == 0)
			{
				uint32 start_address = 0;
				uint32 counter = 0;
				int index = 0;
				for(uint32 i = 0;i<MAX_Count;i++)
				{
					if (user_heap_info[i].free == 1)
					{
						if (counter == frames_needed)
						{
							start_address = user_heap_info[i-counter].address;
							index = i-counter;
							break;
						}
						counter++;
					}
					else
					{
						if (counter == frames_needed)
						{
							start_address = user_heap_info[i-counter].address;
							index = i-counter;
							break;
						}
					}
				}
				if (frames_needed > counter)
				{
					return NULL;
				}
				for(int i=0;i<frames_needed;i++)
				{
					user_heap_info[index].free = 0;
					user_heap_info[index].size = size;
					index++;
				}
				sys_getSharedObject(ownerEnvID,sharedVarName,(void*)start_address);
				return (void*) start_address;
			}

			else
			{
				uint32 start_ptr = 0;
				uint32 counter = 0;
				uint32 min_counter = 1000000000;
				int index = 0;
				for(uint32 i = 0;i<MAX_Count;i++)
				{
					if (i+1 == MAX_Count && counter != 0)
					{
						if (counter >= frames_needed)
						{
							if (min_counter > counter)
							{
								start_ptr = user_heap_info[i-counter].address;
								min_counter = counter;
								index = i-counter;
							}
						}
					}
					if (user_heap_info[i].free == 1)
					{
						counter++;
					}
					else
					{
						if (counter >= frames_needed)
						{
							if (min_counter > counter)
							{
								start_ptr = user_heap_info[i-counter].address;
								min_counter = counter;
								index = i-counter;
							}
						}
						counter = 0;
					}
				}
				if (frames_needed > counter)
					return NULL;

				for(int i = 0;i<frames_needed;i++)
				{
					user_heap_info[index].free = 0;
					user_heap_info[index].size = size;
					index++;
				}
				sys_getSharedObject(ownerEnvID,sharedVarName,(void*)start_ptr);
				return (void*) start_ptr;
			}
				}
	}
	//This function should find the space for sharing the variable
	// ******** ON 4KB BOUNDARY ******************* //

	//Use sys_isUHeapPlacementStrategyBESTFIT() to check the current strategy

	//change this "return" according to your answer
	return 0;
}

// free():
//	This function frees the allocation of the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from page file and main memory then switch back to the user again.
//
//	We can use sys_freeMem(uint32 virtual_address, uint32 size); which
//		switches to the kernel mode, calls freeMem(struct Env* e, uint32 virtual_address, uint32 size) in
//		"memory_manager.c", then switch back to the user mode here
//	the freeMem function is empty, make sure to implement it.

void free(void* virtual_address)
{
	//TODO: [PROJECT 2019 - MS2 - [5] User Heap] free() [User Side]
	// Write your code here, remove the panic and write your code
	free_frames_user = 1;
	int index = 0;
	for (int i=0;i<MAX_Count;i++)
	{
		if (user_heap_info[i].address == (uint32)virtual_address)
		{
			sys_freeMem((uint32)virtual_address, user_heap_info[i].size);
			index = i;
			break;
		}
	}

	int frames_deleted =  user_heap_info[index].size / PAGE_SIZE;
	if ( user_heap_info[index].size % PAGE_SIZE > 0)
		frames_deleted++;

	for(int i=0;i<frames_deleted;i++)
	{
		user_heap_info[index].free = 1;
		user_heap_info[index].size = 0;
		index++;
	}


	//you should get the size of the given allocation using its address
	//you need to call sys_freeMem()
	//refer to the project presentation and documentation for details
}


//==================================================================================//
//============================== BONUS FUNCTIONS ===================================//
//==================================================================================//

//=============
// [1] sfree():
//=============
//	This function frees the shared variable at the given virtual_address
//	To do this, we need to switch to the kernel, free the pages AND "EMPTY" PAGE TABLES
//	from main memory then switch back to the user again.
//
//	use sys_freeSharedObject(...); which switches to the kernel mode,
//	calls freeSharedObject(...) in "shared_memory_manager.c", then switch back to the user mode here
//	the freeSharedObject() function is empty, make sure to implement it.

void sfree(void* virtual_address)
{
	//TODO: [PROJECT 2019 - BONUS4] Free Shared Variable [User Side]
	// Write your code here, remove the panic and write your code
	panic("sfree() is not implemented yet...!!");

	//	1) you should find the ID of the shared variable at the given address
	//	2) you need to call sys_freeSharedObject()

}


//===============
// [2] realloc():
//===============

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to malloc().
//	A call with new_size = zero is equivalent to free().

//  Hint: you may need to use the sys_moveMem(uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		which switches to the kernel mode, calls moveMem(struct Env* e, uint32 src_virtual_address, uint32 dst_virtual_address, uint32 size)
//		in "memory_manager.c", then switch back to the user mode here
//	the moveMem function is empty, make sure to implement it.

void *realloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS3] User Heap Realloc [User Side]
	// Write your code here, remove the panic and write your code
	panic("realloc() is not implemented yet...!!");

}
