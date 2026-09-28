#pragma once

#include <stdint.h>

#define ASTRAL_SKIN_ABI 1u

extern "C" {

typedef struct AstralSkinSelection {
    uint32_t size;
    uint32_t abi;
    int32_t baseId;
    int32_t extraId;
    int32_t blueprintId;
    int32_t itemId;
    int32_t lootId;
    int32_t assetGroupId;
    int32_t iconId;
    int32_t defaultBroadcast;
    int32_t mythic;
    const char* name;
} AstralSkinSelection;

int AstralSkin_Install(void);
int AstralSkin_Select(const AstralSkinSelection* selection);

}
