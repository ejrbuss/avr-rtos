#pragma once

#ifndef RTOS_MEMORY_H
#define RTOS_MEMORY_H

namespace RTOS {
namespace Memory {

    /**
     * Allocates `bytes` worth of memory (if available) from the RTOS virtual 
     * heap. This memory is permanently reserved for the caller. Produces a 
     * trace alloc if memory is available. Produces a trace error if 
     * RTOS_CHECK_ALLOC is defined and the virtual heap is out of memory. 
     * Allocating more than the virtual heap fits results in undefined 
     * behaviour.
     * 
     * eg.
     *   use RTOS::Memory;
     * 
     *   My_Struct_t * my_instance = static_alloc("my_instance", sizeof My_Struct_t);
     * 
     * @param   const char * handle the debugging handle
     * @param   u16          bytes  the number of bytes to allocate
     * @returns void *              a pointer to the allocated bytes
     */
    void * static_alloc(const char * handle, u16 bytes);

    /**
     * A pool allocator provides fast dynamic memory allocation for fixed sized
     * chunks. A pool allocator is statically allocated and cannot be freed.
     * 
     * eg.
     *   use RTOS::Memory;
     * 
     *   // Create a pool of 10 ints
     *   Pool_t * my_int_pool = Pool::init("int pool", sizeof(int), 10); 
     *   int * a = Pool::alloc(my_int_pool); // Dynamically allocate an int
     *   int * b = Pool::alloc(my_int_pool); // And another
     *   Pool::dealloc(my_int_pool, a);      // Deallocate the first int
     * 
     * Because the pool allocator uses a linked list to maintain the state of
     * the pool their are pointers from one chunk in the pool to the next. While
     * allocated these pointers can be used indirectly to create a linked list.
     * 
     * eg.
     *   use RTOS::Memory;
     * 
     *   Pool_t * my_pool = Pool::init("int pool", sizeof(int), 10);
     *   int * my_linked_list;
     *   // Construct a list of two with one cons cell
     *   my_linked_list = Pool::cons(Pool::alloc(my_pool), Pool::alloc(my_pool));
     *   *my_linked_list = 1;            // Set the first item to 1
     *   *Pool::cdr(my_linked_list) = 2; // Set the second itme to 2
     */
    typedef struct Pool_t Pool_t;
    struct Pool_t {
        u8 chunk;        // The number of bytes in each chunk
        u8 chunks;       // The number of chunks
        // "hidden" fields
        struct {
            u8 * data;   // The memory buffer
            void * head; // The current free head
        } impl;
    };

    namespace Pool {
        
        /**
         * Allocates a new pool with `chunks` number of chunks, each of size
         * `chunk`. Returns a pointer to the pool.
         * 
         * @param  const char * handle the debugging handle
         * @param  u8           chunk  the size of each chunk
         * @param  u8           chunks the number of chunks
         * @retuns Pool_t *            a pointer to the pool.
         */
        Pool_t * init(const char * handle, u8 chunk, u8 chunks);

        /**
         * Allocates a chunk and returns a pointer to it. If the pool is out of 
         * memory and RTOS_CHECK_POOL is enabled an error trace will be 
         * produced, otherwise this function is considered 
         * undefined at this point.
         * 
         * @param   Pool_t * pool the pool to allocate from
         * @returns void *        a chunk
         */
        void * alloc(Pool_t * pool);

        /**
         * Deallocates a chunk (making it available to be allocated again). No
         * references to this chunk should be maintained after deallocation.
         * 
         * @param Pool_t * pool  the pool the chunk is from
         * @param void *   chunk the chunk to Deallocate
         */
        void dealloc(Pool_t * pool, void * chunk);

        /**
         * If this chunk is part of a cons cell, retrieves its cdr chunk, 
         * otherwise returns NULL.
         * 
         * @param   void * chunk the chunk to get the cdr of
         * @returns void *       the cdr chunk
         */
        void * cdr(void * chunk);

        /**
         * Constructs a cons cell from two chunks, with `chunk_car` pointing
         * its cdr to `chunk_cdr`. Returns `chunk_car``, which is used to 
         * represent the cons cell.
         * 
         * @param   void * chunk_car the car chunk
         * @param   void * chunk_cdr the cdr chunk
         * @returns void *           the cons cell (the car chunk)
         */
        void * cons(void * chunk_car, void * chunk_cdr);

    }

}}

#endif /* RTOS_MEMORY_H */