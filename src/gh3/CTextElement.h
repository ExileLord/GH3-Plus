#pragma once
#include <stdint.h>
#include "gh3\CScreenElement.h"
#include "gh3\CXboxFont.h"
#include "gh3\CXboxText.h"

namespace GH3
{
    /// A UI Text Object.
	class GH3P_API CTextElement : CScreenElement
	{
	public:
		QbKey key1F0;
		CXboxFont *font;
		CXboxText **text;
		CXboxText **shadowText;
		int32_t textCount;
		uint32_t *unkStruct;
		bool shadowEnabled;
		int32_t shadowXOffset;
		int32_t shadowYOffset;
		uint32_t dword214;
		bool overrideEncodedRgba;
		bool storedOverrideRgbaState;
		bool overrideEncodedAlpha;
		bool flag4;
		int32_t fontSpacing;
	};
}