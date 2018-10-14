#pragma once

#include "gh3\SDraw2D.h"
#include <stdint.h>

namespace GH3
{
    /// A class representing a 2D text object about to be drawn to the screen. Unknown if this is Neversoft's, Aspyr's, or someone else's.
	class SText : SDraw2D
	{
	public:
		struct VFTable
		{
			SText *(__thiscall *dtor)(SText *this_, bool onHeap);
			int(__thiscall *vmethod1)(SText *this_);
			int(__thiscall *vmethod2)(SText *this_);
			int(__thiscall *vmethod3)(SText *this_);
			int(__thiscall *vmethod4)(SText *this_);
			int(__thiscall *vmethod5)(SText *this_);
		};
		VFTable *vftable;
		SText *stext4;
		bool flag8;
		bool flag9;
		float floatC;
		float float10;
		BlendModeEnum mode;
		uint32_t dword18;
		uint32_t funcPtr;
		uint32_t *xboxWindow2D;
		uint32_t *memory24;
		uint32_t dword28;
		wchar_t *text;
		float float30;
		float float34;
		float float38;
		float float3C;
		float float40;
		float float44;
		uint32_t dword48;
		uint32_t unk4C;
		uint8_t flag50;
		uint8_t flag51;
		uint32_t dword54;
	};
}