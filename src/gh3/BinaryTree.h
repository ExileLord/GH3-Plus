#pragma once

#include "core\GH3Plus.h"
#include "QbKey.h"
#include "QbValueType.h"
#include <stdint.h>


namespace GH3
{
    /// A class representing some sort of binary tree. Details are scarce as this isn't really used from what I've seen.
	struct GH3P_API BinaryTree
	{
	private:
		uint16_t root;
		uint8_t arrayOffset;
		uint8_t byte3; //probably length?
		uint32_t data[1];
	public:
		bool GetTypedValue(QbKey aKey, QbValueType valueType, void *pValue);
	};
}