#pragma once
#include <Windows.h>
#include <stdint.h>

#ifdef GH3PLUSCORE_EXPORTS
	#define GH3PLIBRARY_API __declspec(dllexport) 
#else
	#define GH3PLIBRARY_API __declspec(dllimport) 
#endif

namespace GH3P
{
    class GH3PLIBRARY_API ScopedUnprotect final
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