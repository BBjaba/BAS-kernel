#include "kernelUtil.h"
#include "IO.h"
#include "userinput/mouse.h"
#include "../../desktop/main.h"
#include "time/CMOS/CMOS.h"
#include "time/pit/pit.h"
#include "time/time.h"

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    
    startDesktop();
    
    while (true)
    {
        GlobalRenderer->showPointer();
        standardSleep(0xFFFFFFF);
        GlobalRenderer->hidePointer();
        standardSleep(0xFFFFFFF);
    }
}