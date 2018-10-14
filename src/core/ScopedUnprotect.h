#pragma once
#include "GH3Plus.h"
#include <Windows.h>

namespace GH3P
{
    /// A class that will make protected memory (such as code segments) writable while it is in scope.
    class GH3P_API ScopedUnprotect final
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