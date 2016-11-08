#pragma once

#include "core\GH3Plus.h"
#include "QbValueType.h"
#include "QbKey.h"
#include <stdint.h>

namespace GH3
{

	enum QbNodeFlags : __int8
	{
		QFlagStructItem = 0x1,
		QTypeInt = 0x2,
		QTypeFloat = 0x4,
		QTypeCString = 0x6,
		QTypeWString = 0x8,
		QTypeFloatsX2 = 0xA,
		QTypeFloatsX3 = 0xC,
		QTypeScript = 0xE,
		QTypeCFunc = 0x10,
		QTypeUnk9 = 0x12,
		QTypeQbStruct = 0x14,
		QTypeQbArray = 0x18,
		QTypeQbKey = 0x1A,
		QTypeUnk20 = 0x28,
		QTypeUnk21 = 0x2A,
		QTypeUnk22 = 0x2C,
		QTypeUnk23 = 0x2E,
		QTypeStringPointer = 0x34,
		QTypeDictionary = 0x36,
	};

	struct GH3P_API QbStructNode
	{
	private:
		uint8_t unkFlag0;
		QbNodeFlags flags;
		uint16_t word2;
		uint32_t key;
		uint32_t value;
		QbStructNode *next;

	public:
		
	};


	struct GH3P_API QbStruct
	{
	private:
		uint32_t unknown;
		QbStructNode *first;

	public:
		bool GetTypedValue(QbKey qbKey, QbValueType valueType, uint32_t *value);
		char method_587A50(QbKey aKey, QbValueType valueType, uint32_t *outValue, int a5);
	};

}