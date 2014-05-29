#include "stackallocator.h"

StackAllocator::StackAllocator()
{
}

/* use pre allocated retail memory */
StackAllocator::StackAllocator(void *memory, size_t size) : initialized(false) {
    this->initialize(memory, size);
}

StackAllocator::~StackAllocator() {
    if(!preallocated && initialized)
        free((void *)mem_start);
}

// TODO: add some proper error codes sometime
bool StackAllocator::initialize(size_t size) {
    if(initialized)
        return false;

    mem_start = (uint32_t)malloc(size);
    if(!mem_start)
        return false;

    total_mem_m = size;
    free_mem_m = size;
    used_mem_m = 0;
    marker = mem_start;

    preallocated = false;
    initialized = true;

    return true;
}

bool StackAllocator::initialize(void *memory, size_t size) {
    if(initialized)
        return false;

    mem_start = (uint32_t)memory;
    if(!mem_start)
        return false;

    total_mem_m = size;
    free_mem_m = size;
    used_mem_m = 0;
    marker = mem_start;

    preallocated = true;
    initialized = true;

    return true;
}

void *StackAllocator::push(size_t size) {
    if(size+4 > free_mem_m || !initialized)
        return NULL;

    free_mem_m -= size+4;
    used_mem_m += size+4;

    return (void *)NULL;
}

void StackAllocator::pop() {
    if(size+4 > free_mem_m || !initialized)
        return;

    free_mem_m -= size+4;
    used_mem_m += size+4;

    return;
}

void *StackAllocator::back() {
    return (void *)mem_end;
}

void StackAllocator::clear() {
    used_mem_m = 0;
    free_mem_m = total_mem_m;
}

void StackAllocator::cleanup() {
    used_mem_m = 0;
    if(!preallocated && initialized) {
        initialized = false;
        free((void *)mem_start);
    }
}

