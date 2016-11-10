#include "QbStruct.h"
#include "GlobalMap.h"
#include "BinaryTree.h"

namespace GH3
{
	bool QbStruct::GetTypedValue(QbKey qbKey, QbValueType valueType, void * value)
	{
		QbStructNode *node; // edi@1
		void *value2; // ebp@2
		QbValueType type; // al@3 MAPDST
		HashMapNode *item; // eax@7
		HashMapNode *item2; // esi@10
		QbValueType itemType; // cl@18
		QbStructNode *node2; // eax@43
		bool success; // [sp+Bh] [bp-1h]@1
					  // Is this the GetDWord variant for ints?
		node = this->first;
		success = false;                                  // 
													  // 
		if (!node)
			return success;

		value2 = value;
		uint32_t *iValue = static_cast<uint32_t *>(value);
		float *fValue = static_cast<float *>(value);

		uint32_t tmp;

													  
		while (1)
		{
			type = node->Type();
														// 
			if (type == TypeQbMap)
				break;                                    // 
														  // 
			if (type == TypeBinaryTree2 || type == TypeBinaryTree1)
			{
				BinaryTree *tree = reinterpret_cast<BinaryTree *>(node->value);
				if (tree->GetTypedValue(qbKey, valueType, value))
					goto CONTINUE_LOOP;
			}                                           // 
														// 
			else if (type == TypeStringPointer)
			{
				for (item = GlobalMap::GetEntry(node->value); item; item = item->nextItem)
				{
					if (item->key == static_cast<QbKey>(node->value))
						break;
				}
				item2 = item;
				if (item && ((type = item->itemType, type == TypeQbKey) || type == TypeStringPointer))
				{
					while (1)
					{
						item = GlobalMap::GetEntry(item2->value); // g_HashMap[item2->value.key & 0x7FFF];
						if (!item)
							break;
						while (item->key != static_cast<QbKey>(item2->value))
						{
							item = item->nextItem;
							if (!item)
								goto LABEL_22;
						}
						if (!item)
							break;
						itemType = item->itemType;

						if (itemType != TypeQbKey && itemType != TypeStringPointer)
							goto LABEL_21;
						item2 = item;
					}
				}
				else
				{
				LABEL_21:
					item2 = item;
				}
			LABEL_22:
				if (!item2)
					goto CONTINUE_LOOP_B;

				QbStruct *st = reinterpret_cast<QbStruct *>(item2->value);
				if (node->key == 0
					&& item2->itemType == TypeQbStruct
					&& st->GetTypedValue(qbKey, valueType, &tmp))
				{
					*iValue = tmp;
					success = true;
					if (this->unk3)
						return success;
				}
				if (node->key != qbKey)
					goto CONTINUE_LOOP_B;
				if (item2->itemType == valueType)
				{
					*iValue = item2->value;
					goto CONTINUE_LOOP;
				}
				if (item2->itemType == TypeInt && valueType == TypeFloat)
				{
					*fValue = (float)(signed int)item2->value;
					goto CONTINUE_LOOP;
				}
			}
			else if (node->key == qbKey)
			{
				if (node->Type() == valueType)
				{
					*iValue = node->value;
					goto CONTINUE_LOOP;
				}
				if (type == TypeInt && valueType == TypeFloat)
				{
					*fValue = (float)(signed int)node->value;
				CONTINUE_LOOP:
					success = true;
					if (this->unk3)
						return success;
					goto CONTINUE_LOOP_B;
				}
			}
		CONTINUE_LOOP_B:
			node = node->next;
			if (!node)
				return success;
		}                                             // 
													  // 
													  // 
													  // 
													  // 
		QbMap *map = reinterpret_cast<QbMap *>(node->value);
		node2 = reinterpret_cast<QbStructNode *>(map->Get(qbKey));
		if (node2)
		{
			if (node2->Type() == valueType)
			{
				*iValue = node2->value;
				return true;
			}
			if (valueType == QbValueType::TypeFloat && node2->Type() == QbValueType::TypeInt)// If valueType is a float but the node is an Int
			{
				*fValue = (float)(signed int)node2->value;// Converting Int to Float
				return true;
			}
		}
		return success;

	}
}