#ifndef DESTACKALLOCATOR_H
#define DESTACKALLOCATOR_H

#include "../globals.h"
#include <stdint.h>
#include <stddef.h>

class deStackAllocator
{
public:
    deStackAllocator();
    deStackAllocator(size_t size);
    deStackAllocator(void *memory, size_t size);
    ~deStackAllocator();

    bool initialize(size_t size);
    bool initialize(void *memory, size_t size);

    void *pushFront(size_t size);
    void *pushBack(size_t size);
    void popFront();
    void popBack();
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

    uint32_t mem_start;
    uint32_t mem_end;
    uint32_t marker_front;
    uint32_t marker_back;
};

#endif // DESTACKALLOCATOR_H
