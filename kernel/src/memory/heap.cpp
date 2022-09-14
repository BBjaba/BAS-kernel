#include "heap.h"
#include "../paging/PageTableManager.h"
#include "../paging/PageFrameAllocator.h"

void* start;
void* end;
HeapSegmentHeader* LastHdr;

void InitializeHeap(void* heapAddress, size_t pageCount){
    void* pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++){
        g_PageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    start = heapAddress;
    end = (void*)((size_t)start + heapLength);
    HeapSegmentHeader* startSeg = (HeapSegmentHeader*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegmentHeader);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    LastHdr = startSeg;
}

void free(void* address){
    HeapSegmentHeader* segment = (HeapSegmentHeader*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void* malloc(size_t size){
    if (size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    HeapSegmentHeader* currentSeg = (HeapSegmentHeader*) start;
    while(true){
        if(currentSeg->free){
            if (currentSeg->length > size){
                currentSeg->Split(size);
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }
            if (currentSeg->length == size){
                currentSeg->free = false;
                return (void*)((uint64_t)currentSeg + sizeof(HeapSegmentHeader));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size);
}

HeapSegmentHeader* HeapSegmentHeader::Split(size_t splitLength){
    if (splitLength < 0x10) return NULL;
    int64_t splitSegLength = length - splitLength - (sizeof(HeapSegmentHeader));
    if (splitSegLength < 0x10) return NULL;

    HeapSegmentHeader* newSplitHdr = (HeapSegmentHeader*) ((size_t)this + splitLength + sizeof(HeapSegmentHeader));
    next->last = newSplitHdr;
    newSplitHdr->next = next;
    next = newSplitHdr;
    newSplitHdr->last = this;
    newSplitHdr->length = splitSegLength;
    newSplitHdr->free = free;
    length = splitLength;

    if (LastHdr == this) LastHdr = newSplitHdr;
    return newSplitHdr;
}

void ExpandHeap(size_t length){
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegmentHeader* newSegment = (HeapSegmentHeader*)end;

    for (size_t i = 0; i < pageCount; i++){
        g_PageTableManager.MapMemory(end, GlobalAllocator.RequestPage());
        end = (void*)((size_t)end + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegmentHeader);
    newSegment->CombineBackward();

}

void HeapSegmentHeader::CombineForward(){
    if (next == NULL) return;
    if (!next->free) return;

    if (next == LastHdr) LastHdr = this;

    if (next->next != NULL){
        next->next->last = this;
    }

    length = length + next->length + sizeof(HeapSegmentHeader);

    next = next->next;
}

void HeapSegmentHeader::CombineBackward(){
    if (last != NULL && last->free) last->CombineForward();
}