#pragma once
#include <stdint.h>
#include "gh3\CRefCounted.h"
#include "gh3\QbKey.h"

namespace GH3
{
    /// Unknown. More specific version of CRefCounted
	class GH3P_API CAliveRefCounted : CRefCounted
	{

	public:
		uint32_t object8;
	};
}
