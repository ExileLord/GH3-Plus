#pragma once
#include "core\GH3Plus.h"
#include <stdint.h>

namespace GH3
{
    /// Base class of a large amount of classes in Neversoft's engine. Indicates a class that is supposedly reference counted.
    /// The actual methods to CRefCounted haven't been found yet but you probably don't have to worry about them.
	class GH3P_API CRefCounted
	{
	public:
		struct VFTable
		{
			int(__thiscall *dtor)(CRefCounted *this_, bool onHeap);
		};

		void *vftable;
		uint32_t dword4;
	};
}