#pragma once
#include <stdint.h>
#include "acpi.h"
#include "paging/PageTableManager.h"
#include "BasicRenderer.h"
#include "cstr.h"
#include "AHCI/ahci.h"

namespace PCI
{
    struct PassDevices
    {
        AHCI::Port* devices[32][32];
    };

    void EnumeratePCI(ACPI::MCFGHeader* mcfg);

    extern const char* deviceClasses[];
    const char* GetVendorName(uint16_t vendorID);
    const char* GetDeviceName(uint16_t vendorID, uint16_t deviceID);
    const char* GetSubclassName(uint8_t classCode, uint8_t subclassCode);
    const char* GetProgIFName(uint8_t classCode, uint8_t subclassCode, uint8_t progIF);
    PassDevices getDevices();
}
