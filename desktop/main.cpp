#include "main.h"

void startDesktop()
{
    GlobalRenderer->Println("PS2 Mouse and Keyboard initialized");
    GlobalRenderer->Next();
    shellInitialize();
}