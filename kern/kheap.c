#include <inc/memlayout.h>
#include <kern/kheap.h>
#include <kern/memory_manager.h>

//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
struct allocation_info
{
	uint32 size;
	uint32 starting_address;
};
uint32 count_allocation_info = 0;

struct allocation_info pages_info [1000];

uint32 free_frames = 0; // if free_frames 0 there is no kfree , if free_frames 1 there is kfree
uint32 starting_ptr = KERNEL_HEAP_START;
void* kmalloc(unsigned int size)
{
	unsigned int frames_needed = size / PAGE_SIZE;
	uint32 * ptr_page_table = NULL;
	uint32 returned_value;
	if (size % PAGE_SIZE > 0)
		{
			frames_needed++;
		}
	// best fit strategy
	if (isKHeapPlacementStrategyBESTFIT()) {
	if (free_frames == 0)
	{
		uint32 counter = 0;
		for(uint32 i = KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
		{
			struct Frame_Info * ptr_frame_info = get_frame_info(ptr_page_directory,(void*)i,&ptr_page_table);
			if (ptr_frame_info == NULL)
			{

				if (counter == frames_needed)
				{
					break;
				}
				counter++;
			}
			else
			{
				if (counter == frames_needed)
				{
					break;
				}
			}
		}
		if (frames_needed > counter)
		{
			return NULL;
		}
		returned_value = starting_ptr;
		pages_info[count_allocation_info].size = size;
		pages_info[count_allocation_info].starting_address = returned_value;
		count_allocation_info++;

		for(uint32 i = 0;i<frames_needed;i++)
		{
			struct Frame_Info * ptr_frame_info =NULL;
			int ret = allocate_frame(&ptr_frame_info);
			ret = map_frame(ptr_page_directory,ptr_frame_info,(void*)starting_ptr,PERM_WRITEABLE);
			starting_ptr += PAGE_SIZE;

		}
	}
	// after unmapping pages

	else
	{
		uint32 start_ptr = 0;
		uint32 counter = 0;
		uint32 min_counter = 1000000000;
		// for the minimum counter that will be equal the number of page that will be used
		// all the virtual addresses are free  == (40959)
		for(uint32 i = KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
		{
			struct Frame_Info * ptr_frame_info = get_frame_info(ptr_page_directory,(void*)i,&ptr_page_table);
			if (i+PAGE_SIZE == KERNEL_HEAP_MAX && counter != 0)
			{
				if (counter >= frames_needed)
				{
					if (min_counter > counter)
					{
						start_ptr = i;
						min_counter = counter;
						start_ptr = start_ptr - (min_counter*PAGE_SIZE);
					}
				}
			}
			if (ptr_frame_info == NULL)
			{
				counter++;
			}
			else
			{
				if (counter >= frames_needed)
				{
					if (min_counter > counter)
					{
						start_ptr = i;
						min_counter = counter;
						start_ptr = start_ptr - (min_counter*PAGE_SIZE);
					}
				}
				counter = 0;
			}
		}
		if (frames_needed > counter)
			return NULL;
		returned_value = start_ptr;
		pages_info[count_allocation_info].size = size;
		pages_info[count_allocation_info].starting_address = start_ptr;
		count_allocation_info++;
		for(int i =0;i<frames_needed;i++)
		{
			struct Frame_Info * ptr_frame_info = NULL;
			int ret = allocate_frame(&ptr_frame_info);
			ret = map_frame(ptr_page_directory,ptr_frame_info,(void*)start_ptr,PERM_WRITEABLE);
			start_ptr += PAGE_SIZE;
		}
	}
	}

	// first fit
	else if (isKHeapPlacementStrategyFIRSTFIT())
	{
			uint32 counter = 0;
			uint32 start_ptr = 0;
			int check = 0;
			for(uint32 i = KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
			{
				uint32 * ptr_page_table = NULL;
				struct Frame_Info * ptr_frame_info = get_frame_info(ptr_page_directory,(void*)i,&ptr_page_table);
				if (ptr_frame_info == NULL)
				{
					if (counter == frames_needed)
					{
						check = 1;
						start_ptr = i - (counter*PAGE_SIZE);
						break;
					}
					counter++;
				}
				else
				{
					counter = 0;
				}
			}
			if(check == 0)
				return NULL;
			returned_value = start_ptr;
			pages_info[count_allocation_info].size = size;
			pages_info[count_allocation_info].starting_address = returned_value;
			count_allocation_info++;

			for(uint32 i = 0;i<frames_needed;i++)
			{
				uint32 * ptr_page_table = NULL;
				struct Frame_Info * ptr_frame_info = NULL;
				int ret = allocate_frame(&ptr_frame_info);
				ret = map_frame(ptr_page_directory,ptr_frame_info,(void*)start_ptr,PERM_WRITEABLE);
				start_ptr += PAGE_SIZE;

			}
	}
	return (void*)returned_value;
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kmalloc()
	// Write your code here, remove the panic and write your code
	//kpanic_into_prompt("kmalloc() is not implemented yet...!!");


	//NOTE: Allocation is based on BEST FIT strategy
	//NOTE: All kernel heap allocations are multiples of PAGE_SIZE (4KB)
	//refer to the project presentation and documentation for details


	//TODO: [PROJECT 2019 - BONUS1] Implement the FIRST FIT strategy for Kernel allocation
	// Beside the BEST FIT
	// use "isKHeapPlacementStrategyFIRSTFIT() ..." functions to check the current strategy

	//change this "return" according to your answer

}

void kfree(void* virtual_address)
{
	free_frames = 1;
	uint32 size = 0;
	int deleted_index;
	for(int i=0;i<count_allocation_info;i++)
	{
		if (virtual_address == (void *)pages_info[i].starting_address)
		{
			size = pages_info[i].size;
			deleted_index = i;
			break;
		}
	}
	// shifting elements in array of struct after free frames
	for(int i=deleted_index;i<count_allocation_info;i++)
	{
		pages_info[i].size = pages_info[i+1].size;
		pages_info[i].starting_address = pages_info[i+1].starting_address;
	}
	count_allocation_info--;
	uint32 deleted_frames = size / PAGE_SIZE;
	if (size % PAGE_SIZE > 0)
	{
		deleted_frames++;
	}
	for (int i =0;i<deleted_frames;i++)
	{
		unmap_frame(ptr_page_directory,virtual_address);
		virtual_address+=PAGE_SIZE;
	}
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kfree()
	// Write your code here, remove the panic and write your code
	//panic("kfree() is not implemented yet...!!");

	//you need to get the size of the given allocation using its address
	//refer to the project presentation and documentation for details

}

unsigned int kheap_virtual_address(unsigned int physical_address)
{
	uint32 *ptr_page_table = NULL;
	struct Frame_Info *ptr_frame_info=NULL;

	for(int i =0;i<count_allocation_info;i++)
	{
		int frames_count = pages_info[i].size/PAGE_SIZE;
		if (pages_info[i].size%PAGE_SIZE > 0)
			frames_count++;
		uint32 temp_address = pages_info[i].starting_address;
		for(int j = 0;j<frames_count;j++)
		{
			ptr_frame_info = get_frame_info(ptr_page_directory,(void*)temp_address,&ptr_page_table);
			if (ptr_frame_info != NULL)
			{
				uint32 phy = to_physical_address(ptr_frame_info);
				if (phy == physical_address)
					return temp_address;
			}
			temp_address+=PAGE_SIZE;
		}
	}
	/*
	for(uint32 i=KERNEL_HEAP_START;i<KERNEL_HEAP_MAX;i+=PAGE_SIZE)
	{
		uint32 * ptr_page_table = NULL;
		struct Frame_Info*ptr_frame_info = get_frame_info(ptr_page_directory,(void*)i,&ptr_page_table);
		if (ptr_frame_info != NULL)
		{
			uint32 phys_address = to_physical_address(ptr_frame_info);
			if (phys_address == physical_address)
			{
				return i;
				break;
			}
		}
	}*/
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_virtual_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_virtual_address() is not implemented yet...!!");

	//return the virtual address corresponding to given physical_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return 0;
}

unsigned int kheap_physical_address(unsigned int virtual_address)
{
	if (virtual_address >= KERNEL_HEAP_START && virtual_address <= KERNEL_HEAP_MAX)
	{
		uint32 * ptr_page_table = NULL;
		struct Frame_Info * ptr_frame_info = get_frame_info(ptr_page_directory,(void*)virtual_address,&ptr_page_table);
		if (ptr_frame_info != NULL)
		{
			uint32 physical_address = to_physical_address(ptr_frame_info);
			return physical_address;
		}
	}
	//TODO: [PROJECT 2019 - MS1 - [1] Kernel Heap] kheap_physical_address()
	// Write your code here, remove the panic and write your code
	//panic("kheap_physical_address() is not implemented yet...!!");

	//return the physical address corresponding to given virtual_address
	//refer to the project presentation and documentation for details

	//change this "return" according to your answer

	return 0;
}


//=================================================================================//
//============================== BONUS FUNCTION ===================================//
//=================================================================================//
// krealloc():

//	Attempts to resize the allocated space at "virtual_address" to "new_size" bytes,
//	possibly moving it in the heap.
//	If successful, returns the new virtual_address, in which case the old virtual_address must no longer be accessed.
//	On failure, returns a null pointer, and the old virtual_address remains valid.

//	A call with virtual_address = null is equivalent to kmalloc().
//	A call with new_size = zero is equivalent to kfree().

void *krealloc(void *virtual_address, uint32 new_size)
{
	//TODO: [PROJECT 2019 - BONUS2] Kernel Heap Realloc
	// Write your code here, remove the panic and write your code
	if (new_size == 0)
	{
		kfree(virtual_address);
		return NULL;
	}
	if (virtual_address == NULL)
	{
		uint32 temp = (uint32)kmalloc(new_size);
		return (void*)temp;
	}
	// two cases size smaller than , size greater than
	int index;
	uint32 *ptr = NULL;
	for (int i =0;i<count_allocation_info;i++)
	{
		if((void*)pages_info[i].starting_address == virtual_address)
		{
			index = i;
			break;
		}
	}
	// if size equal or smaller than the current size
	if (pages_info[index].size >= new_size)
	{
		return(void*) pages_info[index].starting_address;
	}
	// if size greater than the current size
	else
	{
		int current_frame = pages_info[index].size / PAGE_SIZE;
		if (pages_info[index].size % PAGE_SIZE > 0)
			current_frame++;

		int new_frame = new_size / PAGE_SIZE;
		if (new_size % PAGE_SIZE > 0)
			new_frame++;

		int frames_needed = new_frame-current_frame;
		uint32 end_address = pages_info[index].starting_address + (current_frame*PAGE_SIZE);
		uint32 temp_address = end_address;
		int counter = 0;
		for(int i =0;i<frames_needed;i++)
		{
			uint32 *ptr_page_table = NULL;
			struct Frame_Info*ptr_frame = get_frame_info(ptr_page_directory,(void*)end_address,&ptr_page_table);
			if (ptr_frame == NULL)
				counter++;
			end_address+=PAGE_SIZE;
		}
		if (counter == frames_needed)
		{
			for(int i=0;i<frames_needed;i++)
			{
				uint32 *ptr_page_table = NULL;
				struct Frame_Info*ptr_frame;
				int ret = allocate_frame(&ptr_frame);
				ret = map_frame(ptr_page_directory,ptr_frame,(void*)temp_address,PERM_WRITEABLE);
				temp_address+=PAGE_SIZE;
			}
			temp_address = temp_address - (new_frame*PAGE_SIZE);
			pages_info[index].size = new_size;
			pages_info[index].starting_address = temp_address;
			return (void*)temp_address;
		}
		else
		{
			/*int sizee = new_size - pages_info[index].size;
			int count = pages_info[index].size / PAGE_SIZE;
			if (pages_info[index].size % PAGE_SIZE > 0)
				count++;*/
			kfree(virtual_address);
			uint32 temp = (uint32)kmalloc(new_size);
			//temp += (count*PAGE_SIZE);
			cprintf("my address : %x\n",temp);
			return (void*)temp;
		}
	}
	return NULL;
}
