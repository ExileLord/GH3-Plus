#include "QBKey.h"
#include <cstring>

namespace GH3P
{
	static uint32_t crc32(const void* data, size_t length, uint32_t previousCrc32 = 0) {
		const uint32_t Polynomial = 0xEDB88320;

		uint32_t crc = ~previousCrc32;
		unsigned char* current = (unsigned char*)data;
		while (length--)
		{
			crc ^= *current++;
			for (unsigned int j = 0; j < 8; j++)
				crc = (crc >> 1) ^ (-int(crc & 1) & Polynomial);
		}
		return ~crc;
	}

	QBKey::QBKey() : _key(0)
	{
	}

	QBKey::QBKey(uint32_t key) : _key(key)
	{
	}

	QBKey::QBKey(const char *key) : _key(crc32(key, strlen(key)))
	{
	}


	//Operators
	QBKey & QBKey::operator=(const uint32_t rhs)
	{
		_key = rhs;
		return *this;
	}

	bool QBKey::operator<(const QBKey &rhs) const  { return _key < rhs._key; }
	bool QBKey::operator<=(const QBKey &rhs) const { return _key <= rhs._key; }
	bool QBKey::operator>(const QBKey &rhs) const  { return _key > rhs._key; }
	bool QBKey::operator>=(const QBKey &rhs) const { return _key >= rhs._key; }
	bool QBKey::operator==(const QBKey &rhs) const { return _key == rhs._key; }
	bool QBKey::operator!=(const QBKey &rhs) const { return _key != rhs._key; }
}