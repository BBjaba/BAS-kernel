#include "main.h"

void startDesktop(AHCI::device disk)
{
    GlobalRenderer->Println("PS2 Mouse and Keyboard initialized");
    GlobalRenderer->Next();
    setDisk(disk);
    shellInitialize();
}