#pragma once

#include "core\GH3Plus.h"
#include "QbKey.h"
#include <stdint.h>

namespace GH3
{
    /// Neversoft's pair class that is referenced by the scripting engine. A pair of two floats. Also known as CPair
	struct GH3P_API QbPair
	{
		uint8_t gap0[1];
		uint8_t byte1;
		float X;
		float Y;
	};
}