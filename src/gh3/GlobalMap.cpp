#include "GlobalMap.h"

namespace GH3
{

	bool GlobalMap::GetTypedValue(QbKey key, QbValueType type, void * pValue)
	{
		HashMapNode *node = GetEntry(key);
		uint32_t *pValue2 = reinterpret_cast<uint32_t *>(pValue); //We don't know what type of data is being stored besides that it's 32 bits but that's all we need.

		while (node != nullptr)
		{
			if (node->key == key && node->itemType == type)
			{
				if (pValue2 != nullptr)
					*pValue2 = node->value;
				return true;
			}
			node = node->nextItem;
		}

		if (pValue2 != nullptr)
			*pValue2 = 0;
		return false;
	}

	//TODO: Update this code to use GetNode
	bool GlobalMap::GetValue(QbKey key, void * pValue, QbValueType * pType)
	{
		HashMapNode *node = GetEntry(key);
		uint32_t *pValue2 = reinterpret_cast<uint32_t *>(pValue); //We don't know what type of data is being stored besides that it's 32 bits but that's all we need.

		while (node != nullptr)
		{
			if (node->key == key)
			{
				if (pValue2 != nullptr)
					*pValue2 = node->value;
				if (pType != nullptr)
					*pType = node->itemType;
				return true;
			}
			node = node->nextItem;
		}

		if (pValue2 != nullptr)
			*pValue2 = 0;
		if (pType != nullptr)
			*pType = QbValueType::TypeUndefined;
		return false;
	}

	//Crude way for setting values or doing horrible things the hashmap structure
	HashMapNode * GlobalMap::GetNode(QbKey key)
	{
		HashMapNode *node = GetEntry(key);

		while (node != nullptr)
		{
			if (node->key == key)
			{
				return node;
			}
			node = node->nextItem;
		}

		return nullptr;
	}

	bool GlobalMap::GetInt(QbKey qbKey, int32_t &value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeInt, &value);
	}

	bool GlobalMap::GetUInt(QbKey qbKey, uint32_t &value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeInt, &value);
	}

	bool GlobalMap::GetFloat(QbKey qbKey, float &value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeFloat, &value);
	}

	bool GlobalMap::GetDouble(QbKey qbKey, double &value)
	{
		float fValue;
		return GetTypedValue(qbKey, QbValueType::TypeFloat, &fValue);
		value = fValue;
	}

	bool GlobalMap::GetString(QbKey qbKey, char *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeCString, &value);
	}

	bool GlobalMap::GetWString(QbKey qbKey, wchar_t *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeWString, &value);
	}

	bool GlobalMap::GetPair(QbKey qbKey, QbPair *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypePair, &value);
	}

	bool GlobalMap::GetPair(QbKey qbKey, float & x, float & y)
	{
		QbPair *value;
		if (GetPair(qbKey, value))
		{
			x = value->X;
			y = value->Y;
			return true;
		}
		return false;
	}

	bool GlobalMap::GetVector(QbKey qbKey, QbVector *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeVector, &value);
	}

	bool GlobalMap::GetVector(QbKey qbKey, float & x, float & y, float & z)
	{
		QbVector *value;
		if (GetVector(qbKey, value))
		{
			x = value->X;
			y = value->Y;
			z = value->Z;
			return true;
		}
		return false;
	}

	bool GlobalMap::GetStruct(QbKey qbKey, QbStruct *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeQbStruct, &value);
	}

	bool GlobalMap::GetArray(QbKey qbKey, QbArray *&value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeQbArray, &value);
	}

	bool GlobalMap::GetQbKey(QbKey qbKey, QbKey &value)
	{
		return GetTypedValue(qbKey, QbValueType::TypeQbKey, &value);
	}

}