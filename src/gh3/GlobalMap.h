#pragma once

#include "QbMap.h"

namespace GH3
{
    /// A node of the GlobalMap class
	struct __declspec(align(4)) HashMapNode
	{
		uint8_t byte0;
		uint8_t byte1;
		QbValueType itemType;
		uint8_t byte3;
		QbKey key;
		uint32_t dword8;
		uint32_t value;
		HashMapNode *nextItem;
	};

    /// A static class that essentially stores almost everything to do with game state that is accessible by scripts.
	class GH3P_API GlobalMap final
	{
	private:
		GlobalMap() = delete;
		~GlobalMap() = delete;
	public:
		inline static constexpr uint32_t Size() { return 15; }
		inline static constexpr uint32_t Length() { return 1 << Size(); }
		inline static constexpr uint32_t IndexMask() { return Length() - 1; }

		inline static HashMapNode ** Nodes() { return reinterpret_cast<HashMapNode **>(0x00C03AA8); };

		inline static HashMapNode * GetEntry(QbKey key) { return Nodes()[key & IndexMask()]; };

		static bool GetTypedValue(QbKey key, QbValueType type, void *pValue);
		static bool GetValue(QbKey key, void *pValue, QbValueType *pType);
		static HashMapNode * GetNode(QbKey key);
		static bool GetInt(QbKey qbKey, int32_t &value);
		static bool GetUInt(QbKey qbKey, uint32_t &value);
		static bool GetFloat(QbKey qbKey, float &value);
		static bool GetDouble(QbKey qbKey, double &value);
		static bool GetString(QbKey qbKey, char *&value);
		static bool GetWString(QbKey qbKey, wchar_t *&value);
		static bool GetPair(QbKey qbKey, QbPair *&value);
		static bool GetPair(QbKey qbKey, float &x, float &y);
		static bool GetVector(QbKey qbKey, QbVector *&value);
		static bool GetVector(QbKey qbKey, float &x, float &y, float &z);
		static bool GetStruct(QbKey qbKey, QbStruct *&value);
		static bool GetArray(QbKey qbKey, QbArray *&value);
		static bool GetQbKey(QbKey qbKey, QbKey &value);

		static uint32_t Insert(uint32_t key, QbValueType type, uint32_t value); //TODO

	};

}