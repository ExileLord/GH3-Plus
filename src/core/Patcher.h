#pragma once
#include <stdint.h>
#include "QBKey.h"

#ifdef GH3PLUSCORE_EXPORTS
	#define GH3PLIBRARY_API __declspec(dllexport) 
#else
	#define GH3PLIBRARY_API __declspec(dllimport) 
#endif

namespace GH3P
{
    class GH3PLIBRARY_API Patcher
    {
    private:
		QBKey _owner;
		Patcher() = delete;
    public:
		Patcher(QBKey owner);
        ~Patcher();

		bool WriteJmp   (void *addr, void *jumpDestination);
		bool WriteInt8  (void *addr, uint8_t value);
		bool WriteInt16 (void *addr, uint16_t value);
		bool WriteInt32 (void *addr, uint32_t value);
		bool WriteInt64 (void *addr, uint64_t value);
		bool WriteArray (void *addr, uint8_t *value, uint32_t length);
		bool WriteNOPs  (void *addr, uint32_t length);
    };
}