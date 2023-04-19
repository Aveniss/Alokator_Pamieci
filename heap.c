//
// Created by kamil on 03.12.2022.
//

#include "heap.h"


struct memory_manager_t memory_manager;

int heap_setup(void) {
    memory_manager.memory_start = custom_sbrk(0);
    if (memory_manager.memory_start == (void *) -1)
        return 1;
    memory_manager.first_memory_chunk = NULL;
    memory_manager.status = -456;
    return 0;
}

void heap_clean(void) {
    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    if ((chunk == NULL) || (chunk->free != 1 && chunk->free != 0)) {
        memory_manager.status = 0;
        return;
    }


    struct memory_chunk_t *helper = NULL;

    while (chunk != NULL) {
        helper = chunk;
        chunk = chunk->next;
    }

    size_t sizeee = (size_t) (((char *) helper + sizeof(struct memory_chunk_t) + 12 + helper->size) -
                              (char *) memory_manager.first_memory_chunk);
    memory_manager.memory_start = custom_sbrk((-1) * sizeee);


    memory_manager.status = 0;
}

int heap_validate(void) {
    int s = memory_manager.status;
    if (s != -456)
        return 2;

    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;

    while (chunk != NULL) {
        if (chunk->code != hash(chunk))
            return 3;
        chunk = chunk->next;
    }
    chunk = memory_manager.first_memory_chunk;
    int pointer;
    while (chunk != NULL) {


        pointer = sizeof(struct memory_chunk_t);

        for (int i = 0; i < 6; ++i) {
            if (*((char *) chunk + pointer) != '#' && chunk->free != 1)
                return 1;
            pointer++;
        }
        pointer += chunk->size;

        for (int i = 0; i < 6; ++i) {
            if (*((char *) chunk + pointer) != '#' && chunk->free != 1)
                return 1;
            pointer++;
        }

        chunk = chunk->next;
    }

    return 0;
}

void *heap_malloc(size_t size) {
    if (size == 0 || heap_validate() != 0)
        return NULL;


    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    struct memory_chunk_t *helper = NULL;
    char *tab = (char *) memory_manager.memory_start;
    if (tab == NULL)
        return NULL;
    if (chunk == NULL) {
        memory_manager.memory_start = custom_sbrk(sizeof(struct memory_chunk_t) + 12 + size);
        if (memory_manager.memory_start == (void *) -1)
            return NULL;
        chunk = (struct memory_chunk_t *) memory_manager.memory_start;
        if (chunk == NULL)
            return NULL;
        memory_manager.first_memory_chunk = chunk;

        chunk->size = size;
        chunk->free = 0;
        chunk->next = NULL;
        chunk->prev = NULL;
        chunk->code = hash(chunk);
        for (int i = 0; i < 6; i++) {
            *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
            *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';
        }
        return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
    } else {
        while (chunk != NULL) {

            if (chunk->free && chunk->size >= size + 12)
                break;
            helper = chunk;
            chunk = chunk->next;
        }
        if (chunk != NULL) {
            chunk->free = 0;
            chunk->size = size;
            chunk->code = hash(chunk);
            for (int i = 0; i < 6; i++) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';
            }
            return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
        } else {

            helper->next = custom_sbrk(sizeof(struct memory_chunk_t) + 12 + size);;
            //memory_manager.memory_start = custom_sbrk(sizeof(struct memory_chunk_t)+12+size);
            if (helper->next == (void *) -1) {
                helper->next = NULL;
                return NULL;
            }
            chunk = helper->next;
            chunk->size = size;
            chunk->free = 0;
            chunk->next = NULL;
            chunk->prev = helper;
            chunk->code = hash(chunk);
            helper->code = hash(helper);
            for (int i = 0; i < 6; i++) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';
            }
            return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
        }
    }
    return NULL;
}

void *heap_calloc(size_t number, size_t sizee) {
    if (sizee == 0 || number == 0 || heap_validate() != 0)
        return NULL;

    size_t size = sizee * number;

    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    struct memory_chunk_t *helper = NULL;
    char *tab = (char *) memory_manager.memory_start;
    if (tab == NULL)
        return NULL;
    if (chunk == NULL) {
        memory_manager.memory_start = custom_sbrk(sizeof(struct memory_chunk_t) + 12 + size);
        if (memory_manager.memory_start == (void *) -1)
            return NULL;
        chunk = (struct memory_chunk_t *) memory_manager.memory_start;
        if (chunk == NULL)
            return NULL;
        chunk->size = size;
        chunk->free = 0;
        chunk->next = NULL;
        chunk->prev = NULL;
        memory_manager.first_memory_chunk = chunk;
        chunk->code = hash(chunk);
        for (int i = 0; i < 6; i++) {
            *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
            *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';
        }
        for (size_t i = 0; i < size; ++i) {
            *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6) = 0;
        }
        return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
    } else {
        while (chunk != NULL) {

            if (chunk->free == 1 && chunk->size >= size + 12)
                break;
            helper = chunk;
            chunk = chunk->next;
        }
        if (chunk != NULL) {
            chunk->free = 0;
            chunk->size = size;
            for (int i = 0; i < 6; i++) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';
            }
            for (size_t i = 0; i < size; ++i) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6) = 0;
            }
            chunk->code = hash(chunk);
            return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
        } else {
            helper->next = custom_sbrk(sizeof(struct memory_chunk_t) + 12 + size);;
            if (helper->next == (void *) -1) {
                helper->next = NULL;
                return NULL;
            }
            chunk = helper->next;
            chunk->size = size;
            chunk->free = 0;
            chunk->next = NULL;
            chunk->prev = helper;
            chunk->code = hash(chunk);
            helper->code = hash(helper);
            for (int i = 0; i < 6; i++) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + size) = '#';

            }
            for (size_t i = 0; i < size; ++i) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6) = 0;
            }
            return (char *) chunk + sizeof(struct memory_chunk_t) + 6;
        }
    }
    return NULL;
}

void *heap_realloc(void *memblock, size_t count) {
    if ((memblock == NULL && count == 0))
        return NULL;
    if (heap_validate() != 0)
        return NULL;
    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    struct memory_chunk_t *helper;
    struct memory_chunk_t *last;
    int x = memory_manager.status;
    size_t size;
    if (x != -456)
        return NULL;
    if (memblock == NULL) {
        return heap_malloc(count);
    }
    int flag = 1;

    while (chunk != NULL) {
        if ((char *) chunk + sizeof(struct memory_chunk_t) + 6 == (char *) memblock) {
            flag = 0;
        }
        last = chunk;
        chunk = chunk->next;
    }
    if (flag)
        return NULL;

    chunk = (struct memory_chunk_t *) ((char *) (memblock) - sizeof(struct memory_chunk_t) - 6);
    if (chunk->size == count)
        return memblock;

    if (count == 0) {
        heap_free(memblock);

        return NULL;
    } else if (count < chunk->size) {


        chunk->size = count;
        for (int i = 0; i < 6; i++) {
            *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
            *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + chunk->size) = '#';
        }
        chunk->code = hash(chunk);
    } else if (count != chunk->size) {
        size = (((int) ((char *) chunk->next - (char *) chunk)) - sizeof(struct memory_chunk_t)) - 12;
        if (chunk->next == NULL) {
            memory_manager.memory_start = custom_sbrk(count - chunk->size);

            if (memory_manager.memory_start == (void *) -1)
                return NULL;
            chunk->size = count;
            for (int i = 0; i < 6; i++) {
                *((char *) chunk + sizeof(struct memory_chunk_t) + i) = '#';
                *((char *) chunk + sizeof(struct memory_chunk_t) + i + 6 + chunk->size) = '#';
            }
            chunk->code = hash(chunk);
        } else if (chunk->next != NULL && chunk->next->free == 1 &&
                   size + sizeof(struct memory_chunk_t) + chunk->next->size >= (count)) {

            heap_free(memblock);
            return heap_malloc(count);

        } else {
            char *ptr = heap_malloc(count);
            chunk = (struct memory_chunk_t *) ((char *) (ptr) - sizeof(struct memory_chunk_t) - 6);
            if (ptr == NULL)
                return NULL;
            helper = (struct memory_chunk_t *) ((char *) (memblock) - sizeof(struct memory_chunk_t) - 6);
            for (size_t i = 0; i < (size_t) helper->size; i++) {
                *(ptr + i) = *((char *) memblock + i);
            }


            chunk->code = hash(chunk);
            last->code = hash(last);
            heap_free(memblock);

            return ptr;
        }

    }


    return memblock;
}

void heap_free(void *memblock) {

    if (heap_validate())
        return;
    struct memory_chunk_t *helper = NULL;
    helper = memory_manager.first_memory_chunk;
    int flag = 1;
    while (helper != NULL) {
        if ((char *) helper + sizeof(struct memory_chunk_t) + 6 == (char *) memblock) {
            flag = 0;
            break;
        }
        helper = helper->next;
    }
    if (flag)
        return;
    if (memblock) {
        struct memory_chunk_t *chunk = (struct memory_chunk_t *) ((char *) memblock - sizeof(struct memory_chunk_t) -
                                                                  6);
        helper = NULL;
        if (chunk->prev == NULL && chunk->next == NULL) {


            if (chunk->free == 1)
                memory_manager.memory_start = custom_sbrk((-1) * (sizeof(struct memory_chunk_t) + chunk->size));
            else
                memory_manager.memory_start = custom_sbrk((-1) * (sizeof(struct memory_chunk_t) + 12 + chunk->size));

            memory_manager.first_memory_chunk = NULL;
            return;
        } else if (chunk->next == NULL) {
            helper = chunk->prev;
            memory_manager.memory_start = custom_sbrk((-1) * (sizeof(struct memory_chunk_t) + 12 + chunk->size));

            helper->next = NULL;
            helper->code = hash(helper);
        } else {
            chunk->size = ((int) ((char *) chunk->next - (char *) chunk)) - sizeof(struct memory_chunk_t);
            chunk->free = 1;
        }

        chunk = memory_manager.first_memory_chunk;

        while (chunk != NULL) {
            if (chunk->free == 1 && chunk->next != NULL && chunk->next->free == 1) {
                helper = chunk->next->next;
                chunk->size = chunk->size + chunk->next->size + sizeof(struct memory_chunk_t);
                chunk->next = helper;
                if (helper != NULL) {
                    helper->prev = chunk;
                    helper->code = hash(helper);
                }
                chunk->next = helper;
                chunk->free = 1;
                chunk->code = hash(chunk);
                if (helper != NULL) {
                    helper->code = hash(helper);
                }

                chunk = memory_manager.first_memory_chunk;
            } else if (chunk->prev == NULL && chunk->next == NULL && chunk->free == 1) {
                memory_manager.memory_start = custom_sbrk((-1) * (sizeof(struct memory_chunk_t) + chunk->size));

                memory_manager.first_memory_chunk = NULL;
                return;
            } else {
                chunk = chunk->next;
            }


        }

        chunk = memory_manager.first_memory_chunk;
        while (chunk != NULL) {
            chunk->code = hash(chunk);
            chunk = chunk->next;
        }

    }
}

size_t heap_get_largest_used_block_size(void) {
    if (heap_validate())
        return 0;
    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    if (chunk == NULL)
        return 0;
    size_t size = 0;
    while (chunk != NULL) {


        if (size < chunk->size && chunk->free != 1) {
            size = chunk->size;
        }

        chunk = chunk->next;
    }
    return size;
}

enum pointer_type_t get_pointer_type(const void *const pointer) {
    if (pointer == NULL)
        return pointer_null;
    if (heap_validate())
        return pointer_heap_corrupted;
    if (memory_manager.status == -456 && memory_manager.first_memory_chunk == NULL)
        return pointer_unallocated;
    struct memory_chunk_t *chunk = memory_manager.first_memory_chunk;
    if ((chunk == NULL) || (chunk->free != 1 && chunk->free != 0))
        return pointer_heap_corrupted;
    char *wsk = NULL;
    while (chunk != NULL) {
        wsk = (char *) chunk;
        for (unsigned int i = 0; i < sizeof(struct memory_chunk_t); ++i) {
            if ((char *) pointer == ((char *) wsk)) {
                if (chunk->free)
                    return pointer_unallocated;
                return pointer_control_block;
            }
            wsk++;
        }


        for (int i = 0; i < 6; i++) {
            if ((char *) pointer == ((char *) wsk)) {
                if (chunk->free)
                    return pointer_unallocated;
                return pointer_inside_fences;
            }
            wsk++;

        }

        for (int i = 0; i < (int) chunk->size; i++) {
            if ((char *) pointer == ((char *) wsk) && i == 0) {
                if (chunk->free)
                    return pointer_unallocated;
                return pointer_valid;
            }


            if ((char *) pointer == ((char *) wsk)) {
                if (chunk->free)
                    return pointer_unallocated;
                return pointer_inside_data_block;
            }
            wsk++;

        }


        for (int i = 0; i < 6; i++) {
            if ((char *) pointer == ((char *) wsk)) {
                if (chunk->free)
                    return pointer_unallocated;
                return pointer_inside_fences;
            }
            wsk++;


        }


        chunk = chunk->next;
    }
    return pointer_unallocated;
}

unsigned int hash(void *chunk) {
    unsigned int sum = 0;
    char *tab = (char *) chunk;
    char x;
    for (int i = 0; i < (int) (sizeof(struct memory_chunk_t) - sizeof(unsigned int)); ++i) {
        x = *(tab + i);
        sum = sum + (unsigned int) x;
    }
    return sum;
}








