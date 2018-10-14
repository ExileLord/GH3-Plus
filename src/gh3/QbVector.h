#pragma once

#include "core\GH3Plus.h"
#include "QbKey.h"
#include <stdint.h>

namespace GH3
{
    /// Neversoft's vector class that is referenced by the scripting engine. A vector of three floats. Also known as CVector
	struct GH3P_API QbVector
	{
		uint8_t gap0[1];
		uint8_t byte1;
		float X;
		float Y;
		float Z;
	};
}