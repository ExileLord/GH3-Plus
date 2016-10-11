#pragma once

#include <stdint.h>

#ifdef GH3PLUSCORE_EXPORTS
	#define GH3PLIBRARY_API __declspec(dllexport) 
#else
	#define GH3PLIBRARY_API __declspec(dllimport) 
#endif

namespace GH3P
{
	struct GH3PLIBRARY_API QBKey final
	{
	private:
		uint32_t _key;
	public:
		QBKey();
		QBKey(uint32_t);
		QBKey(const char *);

		inline operator uint32_t() const { return _key; }
		QBKey& operator=(const uint32_t);
		bool operator<(const QBKey &rhs) const;
		bool operator<=(const QBKey &rhs) const;
		bool operator>(const QBKey &rhs) const;
		bool operator>=(const QBKey &rhs) const;
		bool operator==(const QBKey &rhs) const;
		bool operator!=(const QBKey &rhs) const;
	};
}