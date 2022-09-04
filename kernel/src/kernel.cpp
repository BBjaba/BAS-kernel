#include "kernelUtil.h"
#include "IO.h"
#include "userinput/mouse.h"
#include "../../desktop/main.h"
#include "CMOS.h"

int lastEventTime = getSeconds();
bool lastEvent = false;

void startPointer()
{
    while (true)
    {
        GlobalRenderer->showPointer();
        sleep(0xFFFFFFF);
        GlobalRenderer->hidePointer();
        sleep(0xFFFFFFF);
    }
}

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);
    PageTableManager* pageTableManager = kernelInfo.pageTableManager;

    GlobalRenderer->Print("Kernel Initialized Successfully");
    GlobalRenderer->Next();

    startDesktop();
    
    while (true)
    {
        if (getSeconds() - lastEventTime >= 2)
        {
            GlobalRenderer->Println("heab");
            if (lastEvent) GlobalRenderer->hidePointer();
            else GlobalRenderer->showPointer(); 
            lastEvent = lastEvent ? false : true;
        }
        MouseDataAction();
    }
}