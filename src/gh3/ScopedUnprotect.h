#pragma once
#include <Windows.h>
#include <stdint.h>


namespace gh3p
{
    const uint8_t GH3_MAX_PLAYERS = 2;

    const LPVOID GH3_CODE_START = (LPVOID)0x00401010;
    const SIZE_T GH3_CODE_SIZE = 0x00490000;

    class ScopedUnprotect
    {
    private:
        DWORD oldProtect_;
        LPVOID addr_;
        SIZE_T length_;
    public:
        ScopedUnprotect();
        ScopedUnprotect(LPVOID addr, SIZE_T length);
        ~ScopedUnprotect();
    };
}