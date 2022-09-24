#include "readDisk.h"

bool readDiskSector(AHCI::device readDevice, uint64_t sector, uint64_t sectorCount, void*  buffer)
{
    for (int i = 0; i < 32; i++)
    {
        if (readDevice.device[i]->portType == AHCI::PortType::SATA)
        {
            if (!readDevice.device[i]->Read(sector, sectorCount, buffer))
            {
                return false;
            }
            return true;
        }
    }
    return false;
}