#include "pit.h"
#include "../../math.h"
#include "IO.h"

namespace PIT
{
    double timeSinceBoot;
    uint16_t Divisor = 65535;

    void SleepSeconds(double seconds)
    {
        double startTime = timeSinceBoot;
        while (timeSinceBoot < startTime + seconds) asm("hlt");
    }

    void Sleep(uint64_t milliseconds)
    {
        SleepSeconds((double)milliseconds / 1000);
    }

    void SetDivisor(uint16_t divisor)
    {
        divisor = max(100, divisor);
        Divisor = divisor;
        outb(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        outb(0x40, (uint8_t)(divisor & 0xff00) >> 8);
    }
    
    uint64_t GetFrequency()
    {
        return baseFrequency / Divisor;
    }

    void SetFrequency(uint64_t frequency)
    {
        SetDivisor(baseFrequency / frequency);
    }

    void Tick()
    {
        timeSinceBoot += 1 / (double)GetFrequency();
    }

} // namespace PIT