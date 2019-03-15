#include <RTOS.h>
#include <Private.h>

namespace RTOS {
namespace Memory {

    static u8 virtual_heap[RTOS_VIRTUAL_HEAP];
    static u16 allocated_bytes = 0;

    void * static_alloc(const char * handle, u16 bytes) {

        void * ptr = virtual_heap + allocated_bytes;
        allocated_bytes += bytes;

        #ifdef RTOS_TRACE
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            Registers::trace.tag = Def_Alloc;
            Registers::trace.def.alloc.handle = handle;
            Registers::trace.def.alloc.bytes = bytes;
            trace();
        }
        #endif

        #if defined(RTOS_CHECK_ALL) || defined(RTOS_CsHECK_ALLOC)
        if (allocated_bytes > RTOS_VIRTUAL_HEAP) {
            ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                Registers::trace.tag = Error_Max_Alloc;
                error();
            }
        }
        #endif

        return ptr;
    }

    namespace Pool {

        // Get chunk of a given node
        #define POOL_NODE_CHUNK(node) ((void *) (node + 1))
        // Get the node of a given chunk
        #define POOL_CHUNK_NODE(chunk) (((Pool_Node_t *) chunk) - 1)

        typedef struct Pool_Node_t Pool_Node_t;
        struct Pool_Node_t {
            Pool_Node_t * cdr; // A pointer to the next available chunk's pool_node
        };
        
        Pool_t * init(const char * handle, u8 chunk, u8 chunks) {

            Pool_t * pool = (Pool_t *) static_alloc(handle, sizeof(Pool_t));

            u8 step = chunk + sizeof(Pool_Node_t);

            pool->chunk     = chunk;
            pool->chunks    = chunks;
            pool->impl.data = (u8 *) static_alloc(handle, chunks * step);
            pool->impl.head = (Pool_Node_t *) pool->impl.data;

            // We point each pool_node to the next in the buffer
            Pool_Node_t * prev = (Pool_Node_t *) pool->impl.head;
            int c;
            for (c = 1; c < chunks; c++) {
                Pool_Node_t * next = (Pool_Node_t *) (pool->impl.data + (step * c));
                prev->cdr = next;
                prev = next;
            }
            // The last pool node points at nothing
            prev->cdr = nullptr;

            return pool;
        }

        void * alloc(Pool_t * pool) {

            #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_POOL)
            if (pool == nullptr) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    Registers::trace.tag = Error_Null_Pool;
                    error();
                }
            }
            if (pool->impl.head == nullptr) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    Registers::trace.tag = Error_Max_Pool;
                    error();
                }
            }
            #endif

            Pool_Node_t * node = (Pool_Node_t *) pool->impl.head;
            pool->impl.head = node->cdr;
            // Needed for cons and cdr to work problem free
            node->cdr = nullptr; 
            return POOL_NODE_CHUNK(node);
        }

        void dealloc(Pool_t * pool, void * chunk) {

            #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_POOL)
            if (pool == nullptr) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    Registers::trace.tag = Error_Null_Pool;
                    error();
                }
            }
            #endif

            Pool_Node_t * node = POOL_CHUNK_NODE(chunk);
            node->cdr = (Pool_Node_t *) pool->impl.head;
            pool->impl.head = node;
        }

        void * cdr(void * chunk) {

            #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_POOL)
            if (chunk == nullptr) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    Registers::trace.tag = Error_Null_Pool;
                    error();
                }
            }
            #endif
            Pool_Node_t * node = POOL_CHUNK_NODE(chunk);
            if (node->cdr == nullptr) {
                return nullptr;
            }
            return POOL_NODE_CHUNK(node->cdr);
        }

        void * cons(void * chunk_car, void * chunk_cdr) {

            #if defined(RTOS_CHECK_ALL) || defined(RTOS_CHECK_POOL)
            if (chunk_car == nullptr) {
                ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
                    Registers::trace.tag = Error_Null_Pool;
                    error();
                }
            }
            #endif

            if (chunk_cdr == nullptr) {
                POOL_CHUNK_NODE(chunk_car)->cdr = nullptr;
            } else {
                POOL_CHUNK_NODE(chunk_car)->cdr = POOL_CHUNK_NODE(chunk_cdr);
            }
            return chunk_car;
        }

    }

}}