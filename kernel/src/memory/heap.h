#pragma once
#include <stdint.h>
#include <stddef.h>

struct HeapSegmentHeader
{
    size_t length;
    HeapSegmentHeader* next;
    HeapSegmentHeader* last;
    bool free;
    void CombineForward();
    void CombineBackward();
    HeapSegmentHeader* Split(size_t segmentLength);
};

void InitializeHeap(void* heapAddress, size_t pageCount);

void* malloc(size_t size);
void free(void* address);

void ExpandHeap(size_t length);