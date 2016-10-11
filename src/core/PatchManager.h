#pragma once

#include <vector>
#include <map>
#include "QBKey.h"

#ifdef GH3PLUSCORE_EXPORTS
	#define GH3PLIBRARY_API __declspec(dllexport) 
#else
	#define GH3PLIBRARY_API __declspec(dllimport) 
#endif

namespace GH3P
{

	class GH3PLIBRARY_API PatchManager final
	{
	private:
		PatchManager() = delete;

	public:
		static void FindPatch(const uint8_t *start, uint32_t size, uint8_t* &patchStart, uint32_t &patchOwner);
		static bool ApplyPatch(const std::vector<uint8_t> &src, uint8_t *dest, uint32_t ownerID);
		static bool ApplyPatch(const uint8_t * src, uint8_t *dest, uint32_t size, uint32_t ownerID);
		static bool RemovePatch(uint8_t *dest);
		static bool RemovePatches(uint32_t ownerID);
	};

}