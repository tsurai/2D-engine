#ifndef STACKALLOCATOR_H
#define STACKALLOCATOR_H

#include "../globals.h"
#include <stdint.h>
#include <stddef.h>

class StackAllocator
{
public:
    StackAllocator();
    StackAllocator(void *memory, size_t size);
    ~StackAllocator();

    bool initialize(size_t size);
    bool initialize(void *memory, size_t size);

    void *push(size_t size);
    void pop();
    void *back();
    void *front();
    void clear();
    void cleanup();

    READ_ONLY(size_t, total_mem)
    READ_ONLY(size_t, used_mem)
    READ_ONLY(size_t, free_mem)
private:
    bool initialized;
    bool preallocated;

    unsigned long size;
    uint32_t marker;
    uint32_t mem_start;
};

#endif // STACKALLOCATOR_H
