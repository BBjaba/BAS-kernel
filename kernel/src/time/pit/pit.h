#pragma once
#include <stdint.h>

namespace PIT
{
    extern double timeSinceBoot;
    const uint64_t baseFrequency = 1193182;

    void SleepSeconds(double seconds);
    void Sleep(uint64_t milliseconds);

    void SetDivisor(uint16_t divisor);
    uint64_t GetFrequency();
    void SetFrequency(uint64_t frequency);
    void Tick();
}