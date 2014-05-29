#include "destackallocator.h"
#include "../globals.h"
#include "engine.h"

// TODO: throw exceptoin
/* allocate retail memory */
deStackAllocator::deStackAllocator() : initialized(false) {
}

deStackAllocator::deStackAllocator(size_t size) : initialized(false) {
    this->initialize(size);
}

/* use pre allocated retail memory */
deStackAllocator::deStackAllocator(void *memory, size_t size) : initialized(false) {
    this->initialize(memory, size);
}

deStackAllocator::~deStackAllocator() {
    if(!preallocated && initialized)
        free((void *)mem_start);
}

// TODO: add some proper error codes sometime
bool deStackAllocator::initialize(size_t size) {
    if(initialized)
        return false;

    mem_start = (uint32_t)malloc(size);
    if(!mem_start)
        return false;

    mem_end = (mem_start + size);
    total_mem_m = size;
    free_mem_m = size;
    used_mem_m = 0;
    marker_front = mem_start;
    marker_back = mem_end;

    preallocated = false;
    initialized = true;

    return true;
}

bool deStackAllocator::initialize(void *memory, size_t size) {
    if(initialized)
        return false;

    mem_start = (uint32_t)memory;
    if(!mem_start)
        return false;

    mem_end = (mem_start + size);
    total_mem_m = size;
    free_mem_m = size;
    used_mem_m = 0;
    marker_front = mem_start;
    marker_back = mem_end;

    preallocated = true;
    initialized = true;

    return true;
}

void *deStackAllocator::pushFront(size_t size) {
    qDebug("allocate %d bytes from 0x%x to 0x%x", size, marker_front, marker_front+size);

    if(size+4 > free_mem_m || !initialized)
        return NULL;

    *((uint32_t *)(marker_front+size)) = marker_front;

    free_mem_m -= size+4;
    used_mem_m += size+4;

    marker_front = (marker_front+size+4);
    return (void *)(marker_front-size-4);
}

void *deStackAllocator::pushBack(size_t size) {
    qDebug("allocate %d bytes from 0x%x to 0x%x", size, marker_back-size, marker_back);

    if(size+4 > free_mem_m || !initialized)
        return NULL;

    *((uint32_t *)(marker_back-size-4)) = marker_back;

    free_mem_m -= size+4;
    used_mem_m += size+4;

    marker_back = (marker_back-size-4);
    return (void *)(marker_back+4);
}


void deStackAllocator::popFront() {
    if(marker_front <= mem_start || !initialized)
        return;

    size_t size = marker_front - *((uint32_t *)(marker_front-4));
    free_mem_m += size;
    used_mem_m -= size;

    marker_front = *((uint32_t *)(marker_front-4));
}

void deStackAllocator::popBack() {
    if(marker_back >= mem_end || !initialized)
        return;

    size_t size = *((uint32_t *)marker_back)-marker_back;
    free_mem_m += size;
    used_mem_m -= size;

    marker_back = *((uint32_t *)marker_back);
}

void *deStackAllocator::front() {
    return (void *)marker_front;
}

void *deStackAllocator::back() {
    return (void *)marker_back;
}

void deStackAllocator::clear() {
    used_mem_m = 0;
    free_mem_m = total_mem_m;
    marker_front = mem_start;
    marker_back = mem_end;
}

void deStackAllocator::cleanup() {
    used_mem_m = 0;
    if(!preallocated && initialized) {
        initialized = false;
        free((void *)mem_start);
    }
}
