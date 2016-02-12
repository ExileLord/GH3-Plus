#include "ScopedUnprotect.h"
#include <Windows.h>

namespace gh3p
{
    void WriteJmp(LPVOID address, LPVOID target)
    {
        gh3p::ScopedUnprotect unprotect(address, 12);

        DWORD jmpSize = (DWORD)target - (DWORD)address - 5;
        BYTE *arr = static_cast<BYTE *>(address);

        arr[0] = 0xE9;
        memcpy(&arr[1], &jmpSize, sizeof(jmpSize));

    }
}