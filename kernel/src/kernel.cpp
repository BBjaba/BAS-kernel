#include "kernelUtil.h"
#include "IO.h"
#include "userinput/mouse.h"
#include "../../desktop/main.h"
#include "time/CMOS/CMOS.h"
#include "time/pit/pit.h"
#include "time/time.h"
#include "pci.h"
#include "filesystem/fat.h"

void* g_data = (void*)0x00500200;

void readEntry(AHCI::device disk, FAT_DirectoryEntry entry)
{
    
}

extern "C" void _start(BootInfo* bootInfo){

    KernelInfo kernelInfo = InitializeKernel(bootInfo);

    AHCI::device disk;

    bool found = false;

    for (int i = 0; i < 32; i++)
    {
        if (PCI::getDevices().devices[i][0]->portType == AHCI::PortType::SATA || PCI::getDevices().devices[i][0]->portType == AHCI::PortType::SATAPI)
        {
            for (int j = 0; j < 32; j++)
            {
                if (PCI::getDevices().devices[i][j]->portType != NULL)
                {
                    if (PCI::getDevices().devices[i][j]->portType == AHCI::PortType::SATA)
                    {
                        if (!found) 
                        {
                            for (int k = 0; k < 32; k++) disk.device[k] = PCI::getDevices().devices[i][k];
                            found = true;
                        }
                    } else if (PCI::getDevices().devices[i][j]->portType == AHCI::PortType::SATAPI)
                    {
                        if (!found) 
                        {
                            for (int k = 0; k < 32; k++) disk.device[k] = PCI::getDevices().devices[i][k];
                            found = true;
                        }
                    }
                }
            }
        } else {
            break;
        }
    }

    if (!FAT_Initialize(disk))
    {
        GlobalRenderer->Println("FAT init error");
    }

    /*FAT_File* fd = FAT_Open(disk, "EFI/");
    FAT_DirectoryEntry entry;
    int i = 0;
    if (fd != NULL)
    {
        while (FAT_ReadEntry(disk, fd, &entry) && i++ < 14)
        {
            bool good = false;
            uint32_t pastColor = GlobalRenderer->Colour;
            if (FAT_isDirectory(entry)) GlobalRenderer->Colour = Cyan;
            for (int i = 0; i < 11; i++) if (entry.Name[i] != 0x00) good = true;
            if (good)
            {
                GlobalRenderer->Print("  ");
                for (int i = 0; i < 11; i++)
                    GlobalRenderer->PutChar(entry.Name[i]);
                GlobalRenderer->Next();
            }
            GlobalRenderer->Colour = pastColor;
        }
        FAT_Close(fd);
    }*/

    /*char buffer[100];
    uint32_t read;
    fd = FAT_Open(disk, "test.txt");
    while ((read = FAT_Read(disk, fd, sizeof(buffer), buffer)))
    {
        for (uint32_t i = 0; i < read; i++)
        {
            GlobalRenderer->PutChar(buffer[i]);
        }
    }
    FAT_Close(fd);*/

    startDesktop(disk);
    
    while (true) asm("hlt");
}