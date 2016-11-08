#include "QbArray.h"

namespace GH3
{

	int * QbArray::Initialize(int size, int type)
	{
		static const void *function = (void *)0x0055DF00;
		__asm jmp function;
	}



	void QbArray::Set(int index, uint32_t value)
	{
		int result;

		if (m_size == 1)
		{
			m_arr = reinterpret_cast<uint32_t *>(value);
		}
		else
		{
			m_arr[index] = value;
		}
	}



	uint32_t QbArray::Get(int index)
	{
		if (m_size != 1)
			return m_arr[index];
		else
			return reinterpret_cast<uint32_t>(m_arr);
	}



	void QbArray::SetType(QbValueType type)
	{
		m_type = type;
	}

}