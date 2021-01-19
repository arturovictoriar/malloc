#include "malloc.h"

static void *begin_h;
void **head_heap = &begin_h;

/**
 * allocate_memory - get bigger the heap memory
 * Return: the last pointer break
 */
void *allocate_memory()
{
	void *last_bp = NULL;

	/* Get bigger the heap memory*/
	last_bp = sbrk(M_CHUNK);
	/* if no able to get bigger the heap return NULL*/
	if (last_bp == ((void *) -1))
		return (NULL);
	/* Clean the new memory of the heap*/
	memset(last_bp, 0, M_CHUNK);

	return (last_bp);
}

/**
 * get_user_ptr - get a free chunk to the user
 * @heap: free pointer of the heap
 * @mem_asked: size of memory requested by the user
 * Return: void pointer with the allocated memory requested by the user
 */
void *get_user_ptr(void *heap, size_t mem_asked)
{
	size_t remaining_mem = 0, c_mem = 0, pre_chunk_h = 0, cur_chunk_h = 0;
	void *u_mem = NULL, *neighbor = NULL, *new_neighbor = NULL;

	pre_chunk_h = ((size_t *) heap)[0], cur_chunk_h = ((size_t *) heap)[1];
	/* If there are a chunck freed before, use it*/
	if (!(pre_chunk_h & 1))
	{
		c_mem = cur_chunk_h - ((cur_chunk_h & 1) ? 1 : 0);
		neighbor = ((char *) heap) + c_mem;
		if (mem_asked + HEADER_C + HEADER_S <= c_mem &&
				PADDING(c_mem - (mem_asked + HEADER_C)) == 0)
		{
			new_neighbor = ((char *) heap) + mem_asked;
			((size_t *) neighbor)[0] -= mem_asked, ((size_t *) new_neighbor)[0] = 0;
			((size_t *) new_neighbor)[1] = c_mem - mem_asked;
			((size_t *) heap)[1] = mem_asked + ((cur_chunk_h & 1) ? 1 : 0);
		}
		else
			((size_t *) neighbor)[0] -= c_mem, ((size_t *) neighbor)[1] -= 1;
		u_mem = (void *) (((char *) heap) + HEADER_C);
		return (u_mem);
	}
	/* Check if there are enough heap memory for allocate the requested memory*/
	remaining_mem = cur_chunk_h - ((cur_chunk_h & 1) ? 1 : 0);
	while (((int) remaining_mem) - ((int) mem_asked) < 0)
	{
		/* Try to get bigger the heap*/
		if (!allocate_memory())
			return (NULL);
		/* Add the new heap memory*/
		remaining_mem += M_CHUNK;
	}
	/* Get the ptr to retrieve the user and set the headers of the user chunks*/
	((size_t *) heap)[0] -= 1;
	((size_t *) heap)[1] = mem_asked + ((cur_chunk_h & 1) ? 1 : 0);
	u_mem = (void *) (((char *) heap) + HEADER_C);
	/* Set the end header of the chain allocated*/
	((size_t *) (((char *) heap) + mem_asked))[0] = 1;
	((size_t *) (((char *) heap) + mem_asked))[1] = remaining_mem - mem_asked;
	return (u_mem);
}

/**
 * lookfor_free_memory - look for a freed chunk in the memory
 * @heap: first pointert of the heap
 * @mem_asked: size of memory requested by the user
 * Return: a void pointer to the free chunk found
 */
void *lookfor_free_memory(void *heap, size_t mem_asked)
{
	size_t pre_chunk_h = 0, cur_chunk_h = 0;
	void *pre_heap = NULL, *pre_pre_heap = NULL;

	/* Look for unallocate memory*/
	pre_chunk_h = ((size_t *) heap)[0], cur_chunk_h = ((size_t *) heap)[1];
	while (!(pre_chunk_h & 1))
	{
		/*If exist contiguos free chunks, merge them*/
		if (pre_heap && ((size_t *) pre_heap)[1] & 1 && cur_chunk_h & 1)
		{
			pre_pre_heap = ((char *) pre_heap) - ((size_t *) pre_heap)[0];
			((size_t *) pre_pre_heap)[1] += pre_chunk_h;
			((size_t *) heap)[0] += ((size_t *) pre_heap)[0];
			heap = pre_pre_heap, cur_chunk_h = ((size_t *) heap)[1];
		}
		/* If find a freed chunk with enough memory, no get memory in the endchain*/
		else if (cur_chunk_h & 1 && mem_asked <= pre_chunk_h)
		{
			heap = ((char *) heap) - pre_chunk_h + ((pre_chunk_h & 1) ? 1 : 0);
			break;
		}
		/* If the prev chunk is free, so fix the chunk size to get the next chunk*/
		pre_heap = heap;
		heap = ((char *) heap) + cur_chunk_h - ((cur_chunk_h & 1) ? 1 : 0);
		pre_chunk_h = ((size_t *) heap)[0], cur_chunk_h = ((size_t *) heap)[1];
	}
	/*If exist a freed chunk before the end allocate, update the end allocate*/
	if ((pre_chunk_h & 1) && (cur_chunk_h & 1))
	{
		((size_t *) pre_heap)[0] += 1;
		((size_t *) pre_heap)[1] += ((size_t *) heap)[1] - 1;
		heap = pre_heap;
	}

	return (heap);
}

/**
 * _malloc - malloc functio functionn
 * @size: number of bytes required to allocate
 * Return: a pointer to memory allocated on success, NULL otherwise
 */
void *_malloc(size_t size)
{
	void *u_mem = NULL, *heap = NULL;
	size_t mem_asked = 0;

	/* If size is 0, No allocate memory*/
	if (size == 0)
		return (NULL);
	/* Get the first pointer for the heap memory*/
	if (!begin_h)
	{
		begin_h = allocate_memory();
		/* If no pointer to heap memory return NULL*/
		if (!begin_h)
			return (NULL);
		/* Inicialize the final header with the remaining heap memory*/
		((size_t *) begin_h)[0] = 1, ((size_t *) begin_h)[1] = M_CHUNK - HEADER_C;
		printf("Entre\n");
	}
	/* Align the requested memory*/
	mem_asked = HEADER_C + size;
	mem_asked += PADDING(mem_asked);
	/* Look for the unallocate memory*/
	heap = lookfor_free_memory(begin_h, mem_asked);
	/* Allocate user requested memory*/
	u_mem = get_user_ptr(heap, mem_asked);

	return (u_mem);
}
