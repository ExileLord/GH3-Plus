#pragma once

//Empty class. All members live in SText currently

namespace GH3
{
	enum class BlendModeEnum
	{
		DIFFUSE = 0x0,
		ADD = 0x1,
		SUBTRACT = 0x2,
		BLEND = 0x3,
		MODULATE = 0x4,
		UNK = 0x5,
	};

    /// Class representing something that can get drawn to the screen. Currently this class is empty because the only subclass GH3+ uses is SText.
	class SDraw2D
	{
	};
}