#pragma once
#include <stdint.h>
#include "gh3\SText.h"
#include "gh3\CXboxFont.h"
#include "gh3\QbKey.h"

namespace GH3
{
    /// A class representing an Xbox 360 text element (PC GH3 is a port of GH3 for the Xbox 360)
	class GH3P_API CXboxText
	{
	public:
		struct __declspec(align(2)) TextTransformation
		{
			__int16 X;
			__int16 Y;
			__int16 scaleX;
			__int16 scaleY;
			__int16 rotation;
		};
		struct VFTable
		{
			CXboxText *(__thiscall CXboxText::*dtor)(CXboxText *this_, bool onHeap);
			int(__thiscall *vmethod1)(CXboxText *this_);
			int(__thiscall *vmethod2)(CXboxText *this_);
			int(__thiscall *CopyAttributesToSText)(CXboxText *this_);
			int(__thiscall *vmethod4)(CXboxText *this_);
			int(__thiscall *CopyWindowToSText)(CXboxText *this_);
		};

		VFTable *vftable;
		CXboxFont *font;
		TextTransformation *coordinates;
		wchar_t *text;
		float float10;
		float float14;
		float float18;
		float float1C;
		float float20;
		float float24;
		uint32_t dword28;
		float float2C;
		float float30;
		float float34;
		uint32_t color;
		QbKey mode;
		uint8_t flag40;
		uint8_t flag41;
		uint32_t dword44;
		uint32_t dword48;
		uint32_t dword4C;
		uint32_t *cwindow2D;
		CXboxText *ctext54;
		SText *stext58;
	};
}