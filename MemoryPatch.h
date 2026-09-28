
#pragma once

#include <vector>
#include "KittyUtils.h"
#include "KittyMemory.h"
using KittyMemory::Memory_Status;
using KittyMemory::ProcMap;

class MemoryPatch {
private:
    uintptr_t _address;
    size_t    _size;

    std::vector<uint8_t> _orig_code;
    std::vector<uint8_t> _patch_code;

    std::string _hexString;

public:
    MemoryPatch();

    MemoryPatch(const char *libraryName, uintptr_t address,
            const void *patch_code, size_t patch_size, bool useMapCache=true);

    MemoryPatch(uintptr_t absolute_address,
            const void *patch_code, size_t patch_size);

    ~MemoryPatch();

    static MemoryPatch createWithHex(const char *libraryName, uintptr_t address, std::string hex, bool useMapCache=true);
    static MemoryPatch createWithHex(uintptr_t absolute_address, std::string hex);

    bool isValid() const;

    size_t get_PatchSize() const;

    uintptr_t get_TargetAddress() const;

    bool Restore();

    bool Modify();

    std::string get_CurrBytes();
};
