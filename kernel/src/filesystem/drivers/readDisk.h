#pragma once
#include <stdint.h>
#include "../../AHCI/ahci.h"

bool readDiskSector(AHCI::device readDevice, uint64_t sector, uint64_t sectorCount, void* buffer);