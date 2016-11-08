#pragma once

#include "core\GH3Plus.h"
#include "QbValueType.h"
#include "QbKey.h"
#include "QbStruct.h"
#include <stdint.h>


namespace GH3
{

	struct GH3P_API QbMap
	{
	private:

		struct __declspec(align(4)) QbMapNode
		{
			QbKey key;
			QbStructNode *node;
			QbMapNode *nextNode;
		};

		uint32_t	m_size;
		QbMapNode	*m_nodes;
		QbNodeFlags	m_typeFlags;
		uint32_t	dwordC;
		uint32_t	dword10;
		uint8_t		byte14;
		uint8_t		m_boolFlag;
		uint8_t		m_keepAliveFlag;
		uint8_t		m_byte17;

	public:
		QbStructNode * GetNode(QbKey key);

		inline uint32_t Size() { return m_size; }
		inline QbValueType Type() { return static_cast<QbValueType>(m_typeFlags >> 1); }
		inline uint32_t IndexMask() { return ( (1 << m_size) - 1); }

	};

}