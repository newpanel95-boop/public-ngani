#pragma once

bool (*orig_bypass)(void *ins);
bool hook_bypass(void *ins) {
    return false;
}

inline void InitializeProtection() {

Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x204218), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x2136a8), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x213f84), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x41BA40), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x44aacc), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x44bc90), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x485B10), (void*)hook_bypass, (void**)&orig_bypass);
Tools::Hook((void*)getAbsoluteAddress(OBFUSCATE("libanogs.so"), 0x4986b0), (void*)hook_bypass, (void**)&orig_bypass);
    
}
