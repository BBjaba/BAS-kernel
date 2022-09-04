#include "Heap.h"

uintptr_t current_break;

void *sbrk(intptr_t incr)
{
    uintptr_t old_break = current_break;
    current_break += incr;
    return (void*) old_break;
}

void* malloc(size_t size) {
    size = (size + sizeof(size_t) + (align_to - 1)) & ~ (align_to - 1);
    free_block* block = free_block_list_head.next;
    free_block** head = &(free_block_list_head.next);
    while (block != 0) {
        if (block -> size >= size) {
            *head = block->next;
            return ((char*)block) + sizeof(size_t);
        }
        head = &(block->next);
        block = block->next;
    }

    block = (free_block*)sbrk(size);
    block -> size = size;

    return ((char*)block) + sizeof(size_t);
}

void free(void* ptr) {
    free_block* block = (free_block*)(((char*)ptr) - sizeof(size_t));
    block -> next = free_block_list_head.next;
    free_block_list_head.next = block;
}