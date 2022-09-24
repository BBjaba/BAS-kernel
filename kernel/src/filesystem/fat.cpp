#include "fat.h"
#include "memory.h"
#include <stddef.h>
#include <stdint.h>
#include "../BasicRenderer.h"
#include "../BSL/string.h"
#include "drivers/readDisk.h"


#define SECTOR_SIZE             512
#define MAX_PATH_SIZE           256
#define MAX_FILE_HANDLES        10
#define ROOT_DIRECTORY_HANDLE   -1
#define FAT_CACHE_SIZE          5

typedef struct 
{
    uint8_t DriveNumber;
    uint8_t _Reserved;
    uint8_t Signature;
    uint32_t VolumeId;         
    uint8_t VolumeLabel[11];   
    uint8_t SystemId[8];
} __attribute__((packed)) FAT_ExtendedBootRecord;

typedef struct 
{
    uint32_t SectorsPerFat;
    uint16_t Flags;
    uint16_t FatVersion;
    uint32_t RootDirectoryCluster;
    uint16_t FSInfoSector;
    uint16_t BackupBootSector;
    uint8_t _Reserved[12];
    FAT_ExtendedBootRecord EBR;

} __attribute((packed)) FAT32_ExtendedBootRecord;

typedef struct 
{
    uint8_t BootJumpInstruction[3];
    uint8_t OemIdentifier[8];
    uint16_t BytesPerSector;
    uint8_t SectorsPerCluster;
    uint16_t ReservedSectors;
    uint8_t FatCount;
    uint16_t DirEntryCount;
    uint16_t TotalSectors;
    uint8_t MediaDescriptorType;
    uint16_t SectorsPerFat;
    uint16_t SectorsPerTrack;
    uint16_t Heads;
    uint32_t HiddenSectors;
    uint32_t LargeSectorCount;

    union {
        FAT_ExtendedBootRecord EBR1216;
        FAT32_ExtendedBootRecord EBR32;
    };

} __attribute__((packed)) FAT_BootSector;


typedef struct
{
    uint8_t Buffer[SECTOR_SIZE];
    FAT_File Public;
    bool Opened;
    uint32_t FirstCluster;
    uint32_t CurrentCluster;
    uint32_t CurrentSectorInCluster;

} FAT_FileData;

typedef struct {
    uint8_t Order;
    int16_t Chars[13];
} FAT_LFNBlock;

typedef struct
{
    union
    {
        FAT_BootSector BootSector;
        uint8_t BootSectorBytes[SECTOR_SIZE];
    } BS;

    FAT_FileData RootDirectory;

    FAT_FileData OpenedFiles[MAX_FILE_HANDLES];

    uint8_t FatCache[FAT_CACHE_SIZE * SECTOR_SIZE];
    uint32_t FatCachePosition;

    FAT_LFNBlock LFNBlocks[FAT_LFN_LAST];
    int LFNCount;

} FAT_Data;

static FAT_Data* g_Data;
static uint32_t g_DataSectionLba;
static uint8_t g_FatType;
static uint32_t g_TotalSectors;
static uint32_t g_SectorsPerFat;

uint32_t FAT_ClusterToLba(uint32_t cluster);

int FAT_CompareLFNBlocks(const void* blockA, const void* blockB)
{
    FAT_LFNBlock* a = (FAT_LFNBlock*)blockA;
    FAT_LFNBlock* b = (FAT_LFNBlock*)blockB;
    return ((int)a->Order) - ((int)b->Order);
}

bool FAT_ReadBootSector(AHCI::device disk)
{
    return readDiskSector(disk, 0, 1, g_Data->BS.BootSectorBytes);
}

bool FAT_ReadFat(AHCI::device disk, size_t lbaIndex)
{
    return readDiskSector(disk, g_Data->BS.BootSector.ReservedSectors + lbaIndex, FAT_CACHE_SIZE, g_Data->FatCache);
}

void FAT_Detect(AHCI::device disk)
{
    uint32_t dataClusters = (g_TotalSectors - g_DataSectionLba) / g_Data->BS.BootSector.SectorsPerCluster;
    if (dataClusters < 0xFF5) 
        g_FatType = 12;
    else if (g_Data->BS.BootSector.SectorsPerFat != 0)
        g_FatType = 16;
    else g_FatType = 32;
}

bool FAT_Initialize(AHCI::device disk)
{
    g_Data = (FAT_Data*)MEMORY_FAT_ADDR;

    if (!FAT_ReadBootSector(disk))
    {
        GlobalRenderer->Println("FAT: read boot sector failed");
        return false;
    }

    g_Data->FatCachePosition = 0xFFFFFFFF;

    g_TotalSectors = g_Data->BS.BootSector.TotalSectors;
    if (g_TotalSectors == 0) {          
        g_TotalSectors = g_Data->BS.BootSector.LargeSectorCount;
    }

    bool isFat32 = false;
    g_SectorsPerFat = g_Data->BS.BootSector.SectorsPerFat;
    if (g_SectorsPerFat == 0) {         
        isFat32 = true;
        g_SectorsPerFat = g_Data->BS.BootSector.EBR32.SectorsPerFat;
    }
    
    uint32_t rootDirLba;
    uint32_t rootDirSize;
    if (isFat32) {
        g_DataSectionLba = g_Data->BS.BootSector.ReservedSectors + g_SectorsPerFat * g_Data->BS.BootSector.FatCount;
        rootDirLba = FAT_ClusterToLba( g_Data->BS.BootSector.EBR32.RootDirectoryCluster);
        rootDirSize = 0;
    }
    else {
        rootDirLba = g_Data->BS.BootSector.ReservedSectors + g_SectorsPerFat * g_Data->BS.BootSector.FatCount;
        rootDirSize = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
        uint32_t rootDirSectors = (rootDirSize + g_Data->BS.BootSector.BytesPerSector - 1) / g_Data->BS.BootSector.BytesPerSector;
        g_DataSectionLba = rootDirLba + rootDirSectors;
    }

    g_Data->RootDirectory.Public.Handle = ROOT_DIRECTORY_HANDLE;
    g_Data->RootDirectory.Public.IsDirectory = true;
    g_Data->RootDirectory.Public.Position = 0;
    g_Data->RootDirectory.Public.Size = sizeof(FAT_DirectoryEntry) * g_Data->BS.BootSector.DirEntryCount;
    g_Data->RootDirectory.Opened = true;
    g_Data->RootDirectory.FirstCluster = rootDirLba;
    g_Data->RootDirectory.CurrentCluster = rootDirLba;
    g_Data->RootDirectory.CurrentSectorInCluster = 0;

    if (!readDiskSector(disk, rootDirLba, 1, g_Data->RootDirectory.Buffer))
    {
        GlobalRenderer->Println("FAT: read root directory failed");
        return false;
    }

    FAT_Detect(disk);

    for (int i = 0; i < MAX_FILE_HANDLES; i++)
        g_Data->OpenedFiles[i].Opened = false;
    g_Data->LFNCount = 0;

    return true;
}

uint32_t FAT_ClusterToLba(uint32_t cluster)
{
    return g_DataSectionLba + (cluster - 2) * g_Data->BS.BootSector.SectorsPerCluster;
}

FAT_File* FAT_OpenEntry(AHCI::device disk, FAT_DirectoryEntry* entry)
{
    int handle = -1;
    for (int i = 0; i < MAX_FILE_HANDLES && handle < 0; i++)
    {
        if (!g_Data->OpenedFiles[i].Opened)
            handle = i;
    }

    if (handle < 0)
    {
        GlobalRenderer->Println("FAT: out of file handles");
        return (FAT_File*)false;
    }

    FAT_FileData* fd = &g_Data->OpenedFiles[handle];
    fd->Public.Handle = handle;
    fd->Public.IsDirectory = (entry->Attributes & FAT_ATTRIBUTE_DIRECTORY) != 0;
    fd->Public.Position = 0;
    fd->Public.Size = entry->Size;
    fd->FirstCluster = entry->FirstClusterLow + ((uint32_t)entry->FirstClusterHigh << 16);
    fd->CurrentCluster = fd->FirstCluster;
    fd->CurrentSectorInCluster = 0;

    if (!readDiskSector(disk, FAT_ClusterToLba(fd->CurrentCluster), 1, fd->Buffer))
    {
        GlobalRenderer->Println("FAT: open entry failed - read error cluster="); GlobalRenderer->Print(to_string((uint64_t)fd->CurrentCluster)); GlobalRenderer->Print(" lba="); GlobalRenderer->Print(to_string((uint64_t)FAT_ClusterToLba(fd->CurrentCluster)));
        for (int i = 0; i < 11; i++)
            GlobalRenderer->PutChar(entry->Name[i]);
            GlobalRenderer->Next();
        GlobalRenderer->Next();
        return (FAT_File*)false;
    }

    fd->Opened = true;
    return &fd->Public;
}

uint32_t FAT_NextCluster(AHCI::device disk, uint32_t currentCluster)
{    
    uint32_t fatIndex;
    if (g_FatType == 12) {
        fatIndex = currentCluster * 3 / 2;
    }
    else if (g_FatType == 16) {
        fatIndex = currentCluster * 2;
    }
    else {
        fatIndex = currentCluster * 4;
    }

    uint32_t fatIndexSector = fatIndex / SECTOR_SIZE;
    if (fatIndexSector < g_Data->FatCachePosition 
        || fatIndexSector >= g_Data->FatCachePosition + FAT_CACHE_SIZE)
    {
        FAT_ReadFat(disk, fatIndexSector);
        g_Data->FatCachePosition = fatIndexSector;
    }

    fatIndex -= (g_Data->FatCachePosition * SECTOR_SIZE);

    uint32_t nextCluster;
    if (g_FatType == 12) {
        if (currentCluster % 2 == 0)
            nextCluster = (*(uint16_t*)(g_Data->FatCache + fatIndex)) & 0x0FFF;
        else
            nextCluster = (*(uint16_t*)(g_Data->FatCache + fatIndex)) >> 4;
        
        if (nextCluster >= 0xFF8) {
            nextCluster |= 0xFFFFF000;
        }
    }
    else if (g_FatType == 16) {
        nextCluster = *(uint16_t*)(g_Data->FatCache + fatIndex);
        if (nextCluster >= 0xFFF8) {
            nextCluster |= 0xFFFF0000;
        }
    }
    else {
        nextCluster = *(uint32_t*)(g_Data->FatCache + fatIndex);
    }

    return nextCluster;
}

uint32_t FAT_Read(AHCI::device disk, FAT_File* file, uint32_t byteCount, void* dataOut)
{
    FAT_FileData* fd = (file->Handle == ROOT_DIRECTORY_HANDLE) 
        ? &g_Data->RootDirectory 
        : &g_Data->OpenedFiles[file->Handle];

    uint8_t* u8DataOut = (uint8_t*)dataOut;

    if (!fd->Public.IsDirectory || (fd->Public.IsDirectory && fd->Public.Size != 0))
        byteCount = min(byteCount, fd->Public.Size - fd->Public.Position);

    while (byteCount > 0)
    {
        uint32_t leftInBuffer = SECTOR_SIZE - (fd->Public.Position % SECTOR_SIZE);
        uint32_t take = min(byteCount, leftInBuffer);

        memcpy(u8DataOut, fd->Buffer + fd->Public.Position % SECTOR_SIZE, take);
        u8DataOut += take;
        fd->Public.Position += take;
        byteCount -= take;

        if (leftInBuffer == take)
        {
            if (fd->Public.Handle == ROOT_DIRECTORY_HANDLE)
            {
                ++fd->CurrentCluster;

                if (!readDiskSector(disk, fd->CurrentCluster, 1, fd->Buffer))
                {
                    GlobalRenderer->Println("FAT: read error!");
                    break;
                }
            }
            else
            {
                if (++fd->CurrentSectorInCluster >= g_Data->BS.BootSector.SectorsPerCluster)
                {
                    fd->CurrentSectorInCluster = 0;
                    fd->CurrentCluster = FAT_NextCluster(disk, fd->CurrentCluster);
                }

                if (fd->CurrentCluster >= 0xFFFFFFF8)
                {
                    fd->Public.Size = fd->Public.Position;
                    break;
                }

                if (!readDiskSector(disk, FAT_ClusterToLba(fd->CurrentCluster) + fd->CurrentSectorInCluster, 1, fd->Buffer))
                {
                    GlobalRenderer->Println("FAT: read error!");
                    break;
                }
            }
        }
    }

    return u8DataOut - (uint8_t*)dataOut;
}

bool FAT_ReadEntry(AHCI::device disk, FAT_File* file, FAT_DirectoryEntry* dirEntry)
{
    return FAT_Read(disk, file, sizeof(FAT_DirectoryEntry), dirEntry) == sizeof(FAT_DirectoryEntry);
}

void FAT_Close(FAT_File* file)
{
    if (file == NULL) return;
    if (file->Handle == ROOT_DIRECTORY_HANDLE)
    {
        file->Position = 0;
        g_Data->RootDirectory.CurrentCluster = g_Data->RootDirectory.FirstCluster;
    }
    else
    {
        g_Data->OpenedFiles[file->Handle].Opened = false;
    }
}

void FAT_GetShortName(const char* name, char shortName[12])
{
    memset(shortName, ' ', 12);
    shortName[11] = '\0';

    const char* ext = strchr(name, '.');
    if (ext == NULL)
        ext = name + 11;

    for (int i = 0; i < 8 && name[i] && name + i < ext; i++)
        shortName[i] = toupper(name[i]);

    if (ext != name + 11)
    {
        for (int i = 0; i < 3 && ext[i + 1]; i++)
            shortName[i + 8] = toupper(ext[i + 1]);
    }
}

bool FAT_FindFile(AHCI::device disk, FAT_File* file, const char* name, FAT_DirectoryEntry* entryOut)
{
    char shortName[12];
    FAT_DirectoryEntry entry;

    FAT_GetShortName(name, shortName);

    while (FAT_ReadEntry(disk, file, &entry))
    {
        if (memcmp(shortName, entry.Name, 11) == 0)
        {
            *entryOut = entry;
            return true;
        }
    }
    
    return false;
}

FAT_File* FAT_Open(AHCI::device disk, const char* path)
{
    char name[MAX_PATH_SIZE];

    if (path[0] == '/')
        path++;

    FAT_File* current = &g_Data->RootDirectory.Public;

    while (*path) {
        bool isLast = false;
        const char* delim = strchr(path, '/');
        if (delim != NULL)
        {
            memcpy(name, path, delim - path);
            name[delim - path] = '\0';
            path = delim + 1;
        }
        else
        {
            unsigned len = strlen((char*) path);
            memcpy(name, path, len);
            name[len + 1] = '\0';
            path += len;
            isLast = true;
        }

        FAT_DirectoryEntry entry;
        if (FAT_FindFile(disk, current, name, &entry))
        {
            FAT_Close(current);

            if (!isLast && entry.Attributes & FAT_ATTRIBUTE_DIRECTORY == 0)
            {
                GlobalRenderer->Print("FAT: "); GlobalRenderer->Print(name); GlobalRenderer->Println(" not a directory");
                return NULL;
            }

            current = FAT_OpenEntry(disk, &entry);
        }
        else
        {
            FAT_Close(current);

            GlobalRenderer->Print("FAT: "); GlobalRenderer->Print(name); GlobalRenderer->Println(" not found");
            return NULL;
        }
    }

    return current;
}

bool FAT_isDirectory(FAT_DirectoryEntry entry) {return entry.Attributes & FAT_ATTRIBUTE_DIRECTORY;}