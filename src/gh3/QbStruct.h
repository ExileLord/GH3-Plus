#pragma once

#include "core\GH3Plus.h"
#include "QbValueType.h"
#include "QbKey.h"
#include "QbPair.h"
#include "QbVector.h"
#include "QbArray.h"
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
		QTypePair = 0xA,
		QTypeVector = 0xC,
		QTypeScript = 0xE,
		QTypeCFunc = 0x10,
		QTypeUnk9 = 0x12,
		QTypeQbStruct = 0x14,
		QTypeQbArray = 0x18,
		QTypeQbKey = 0x1A,
		QTypeUnk20 = 0x28,
		QTypeUnk21 = 0x2A,
		QTypeBinaryTree1 = 0x2C,
		QTypeBinaryTree2 = 0x2E,
		QTypeStringPointer = 0x34,
		QTypeMap = 0x36,
	};

	


	struct GH3P_API QbStruct
	{
		struct QbStructItem
		{
			uint8_t unkFlag0;
			QbNodeFlags flags;
			uint16_t word2;
			uint32_t key;
			uint32_t value;
			QbStructItem *next;

			inline QbValueType Type() { return static_cast<QbValueType>(flags >> 1); }
		};

	private:
		uint16_t unk0;
		uint8_t unk2; //might be a debug flag to not free or something
		uint8_t unk3;
		QbStructItem *first;

	public:
		QbStruct();
		~QbStruct();

		bool GetTypedValue(QbKey qbKey, QbValueType valueType, void *outValue);
		bool GetInt(QbKey qbKey, int32_t &value);
		bool GetUInt(QbKey qbKey, uint32_t &value);
		bool GetFloat(QbKey qbKey, float &value);
		bool GetDouble(QbKey qbKey, double &value);
		bool GetString(QbKey qbKey, char *&value);
		bool GetWString(QbKey qbKey, wchar_t *&value);
		bool GetPair(QbKey qbKey, QbPair &value);
		bool GetVector(QbKey qbKey, QbVector &value);
		bool GetStruct(QbKey qbKey, QbStruct &value);
		bool GetArray(QbKey qbKey, QbArray &value);
		bool GetQbKey(QbKey qbKey, QbKey &value);

		bool ContainsItem(QbKey qbKey);
		bool ContainsTypedItem(QbKey qbKey, QbValueType type);

		//void InsertTypedValue(QbKey qbKey, QbValueType valueType, uint32_t value);
		void __thiscall InsertQbStructItem(QbKey qbKey, QbStruct *item);
		void __thiscall InsertQbKeyItem(QbKey qbKey, QbKey item);
	};

}