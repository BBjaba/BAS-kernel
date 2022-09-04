#include "dynamics.h"

void* operator new (size_t size)
{
    return malloc(size);
}


void* operator new[] (size_t size)
{
    return malloc(size);
}


void operator delete (void* target_void)
{
    free(target_void);
}


void operator delete[] (void* target_void)
{
    free(target_void);
}