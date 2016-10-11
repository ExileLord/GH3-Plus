#pragma once

#ifdef GH3PLIBRARY_EXPORTS
	#define GH3PLIBRARY_API __declspec(dllexport) 
#else
	#define GH3PLIBRARY_API __declspec(dllimport) 
#endif

#include "ScopedUnprotect.h"
#include "PatchManager.h"
#include "Patcher.h"

namespace GH3P
{
    void GH3PLIBRARY_API WriteJmp(LPVOID address, LPVOID target);
}
