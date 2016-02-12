#include "ScopedUnprotect.h"
#include <Windows.h>

namespace gh3p
{
    ScopedUnprotect::ScopedUnprotect() : ScopedUnprotect(GH3_CODE_START, GH3_CODE_SIZE)
    {
    }

    ScopedUnprotect::ScopedUnprotect(LPVOID addr, SIZE_T length) : addr_(addr), length_(length)
    {
        VirtualProtect(addr_, length_, PAGE_EXECUTE_READWRITE, &oldProtect_);
    }

    ScopedUnprotect::~ScopedUnprotect()
    {
        VirtualProtect(addr_, length_, oldProtect_, &oldProtect_);
    }
}