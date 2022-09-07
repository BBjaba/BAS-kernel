
#include "memory.h"

uint64_t GetMemorySize(EFI_MEMORY_DESCRIPTOR* mMap, uint64_t mMapEntries, uint64_t mMapDescSize){

    static uint64_t memorySizeBytes = 0;
    if (memorySizeBytes > 0) return memorySizeBytes;

    for (int i = 0; i < mMapEntries; i++){
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)mMap + (i * mMapDescSize));
        memorySizeBytes += desc->numPages * 4096;
    }

    return memorySizeBytes;

}

void memset(void* start, uint8_t value, uint64_t num){
    for (uint64_t i = 0; i < num; i++){
        *(uint8_t*)((uint64_t)start + i) = value;
    }
}

void* memcpy(void* dst, const void* src, uint16_t num)
{
    uint8_t* u8Dst = (uint8_t*)dst;
    const uint8_t* u8Src = (const uint8_t*)src;

    for (uint16_t i = 0; i < num; i++)
        u8Dst[i] = u8Src[i];

    return dst;
}

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* u8Ptr1 = (const uint8_t*)ptr1;
    const uint8_t* u8Ptr2 = (const uint8_t*)ptr2;

    for (uint16_t i = 0; i < num; i++)
        if (u8Ptr1[i] != u8Ptr2[i])
            return 1;

    return 0;
}