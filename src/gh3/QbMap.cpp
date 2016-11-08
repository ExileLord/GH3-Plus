#include "QbMap.h"

namespace GH3
{

	QbStructNode * QbMap::GetNode(QbKey key)
	{
		QbMapNode *node; // eax@2
		QbStructNode *result; // eax@7

		if (m_nodes && (node = &m_nodes[key & IndexMask()]) != 0)
		{
			while (node->key != key || !this->m_boolFlag && !node->node)
			{
				node = node->nextNode;
				if (!node)
					return nullptr;
			}
			return node->node;
		}

		return nullptr;
	}

}