#pragma once
#include <stdint.h>
#include "core\GH3Plus.h"

namespace GH3
{
	namespace Lst
	{
        /// Node of Neversoft's templated linked list
		template<typename T>
		class GH3P_API Node
		{
		public:
			struct VFTable
			{
				Node<T> *(__thiscall *dtor)(bool onHeap);
			};
			void *vftable;
			T *data;
			uint32_t dword8;
			Node<T> *prev;
			Node<T> *next;
		};

        /// Head node of Neversoft's templated linked list
		template<typename T>
		class GH3P_API Head : Node<T>
		{
		public:
			struct VFTable
			{
				Head<T> *(__thiscall *dtor)(bool onHeap);
			};
		};


	}
}


