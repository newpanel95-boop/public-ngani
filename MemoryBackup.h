
#pragma once

#include <vector>

#include "KittyMemory.h"
using KittyMemory::Memory_Status;
using KittyMemory::ProcMap;

class MemoryBackup {
private:
    uintptr_t _address;
    size_t    _size;

    std::vector<uint8_t> _orig_code;

    std::string _hexString;

public:
    MemoryBackup();

    MemoryBackup(const char *libraryName, uintptr_t address, size_t backup_size, bool useMapCache=true);

    MemoryBackup(uintptr_t absolute_address, size_t backup_size);

    ~MemoryBackup();

    bool isValid() const;

    size_t get_BackupSize() const;

    uintptr_t get_TargetAddress() const;

    bool Restore();

    std::string get_CurrBytes();
};
