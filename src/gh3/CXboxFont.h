#pragma once
#include "gh3\QbKey.h"
#include <stdint.h>

namespace GH3
{
    /// A class representing an Xbox 360 font (PC GH3 is a port of GH3 for the Xbox 360)
	class GH3P_API CXboxFont
	{
	public:
		struct VFTable
		{
			CXboxFont *(__thiscall *dtor)(CXboxFont * this_, bool onHeap);
			int(__thiscall *vmethod1)(CXboxFont * this_);
			int(__thiscall *vmethod2)(CXboxFont * this_);
			int(__thiscall *vmethod3)(CXboxFont * this_);
			int(__thiscall *vmethod4)(CXboxFont * this_);
			int(__thiscall *vmethod5)(CXboxFont * this_);
			int(__thiscall *vmethod6)(CXboxFont * this_);
			int(__thiscall *vmethod7)(CXboxFont * this_);
			int(__thiscall *vmethod8)(CXboxFont * this_);
			int(__thiscall *vmethod9)(CXboxFont * this_);
			int(__thiscall *vmethod10)(CXboxFont * this_);
		};

		void *vftable;
		QbKey name;
		uint32_t unknown;
		uint32_t *memory;
		uint32_t zero1;
		uint32_t zero2;
		uint32_t zero3;
		uint32_t zero4;
		uint32_t unk1;
		uint32_t unk2;
		uint32_t zero5;
		uint32_t *ptrToAbove;
	};
}