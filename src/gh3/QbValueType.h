#pragma once

namespace GH3
{

	enum GH3P_API QbValueType : __int8
	{
		TypeInt = 0x1,
		TypeFloat = 0x2,
		TypeCString = 0x3,
		TypeWString = 0x4,
		TypeFloatsX2 = 0x5,
		TypeFloatsX3 = 0x6,
		TypeScript = 0x7,
		TypeCFunc = 0x8,
		TypeUnk9 = 0x9,
		TypeQbStruct = 0xA,
		TypeQbArray = 0xC,
		TypeQbKey = 0xD,
		TypeUnk20 = 0x14,
		TypeUnk21 = 0x15,
		TypeUnk22 = 0x16,
		TypeUnk23 = 0x17,
		TypeStringPointer = 0x1A,
		TypeDictionary = 0x1B,
		TypeQbKeyStringQs = 0x1C,
	};

}