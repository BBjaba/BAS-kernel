#pragma once
#include <stddef.h>
#include "memory/heap.h"

inline void* operator new (size_t size) {return malloc(size);}
inline void* operator new[] (size_t size) {return malloc(size);}
inline void operator delete (void* target_void) {free(target_void);}
inline void operator delete[] (void* target_void) {free(target_void);}
