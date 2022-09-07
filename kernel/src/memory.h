#pragma once

#include <stdint.h>
#include "efiMemory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize);
void memset(void* start, uint8_t value, uint64_t num);
void* memcpy(void* dst, const void* src, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);