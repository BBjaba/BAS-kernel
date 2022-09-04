#pragma once
#include <stddef.h>
#include "Heap.h"

extern void* operator new (size_t size);
extern void* operator new[] (size_t size);
extern void operator delete (void* target_void);
extern void operator delete[] (void* target_void);
