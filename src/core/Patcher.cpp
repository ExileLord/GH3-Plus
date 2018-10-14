#include "Patcher.h"
#include "PatchManager.h"

#include <vector>

namespace GH3P
{

	//Constructors and destructors
    Patcher::Patcher(GH3::QbKey owner) : owner_(owner) {}

	Patcher::~Patcher()
    {
		RemoveAllChanges();
    }

	bool Patcher::RemoveAllChanges()
	{
		return PatchManager::RemovePatches(owner_);
	}

	//Jumps
	bool Patcher::WriteJmp(void *addr, const void *jumpDestination) const
	{
		return WriteJmpVariant<0xE9>(addr, jumpDestination); //0xE9 is the x86 JMP instruction
	}

	bool Patcher::WriteJmpMulti(void * const *addresses, std::size_t addressCount, const void *jumpDestination) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteJmp(addresses[i], jumpDestination))
				return false;
		}

		return true;
	}

	//Calls
	bool Patcher::WriteCall(void *addr, const void * calleeAddress) const
	{
		return WriteJmpVariant<0xE8>(addr, calleeAddress); //0xE8 is the x86 CALL instruction
	}

	bool Patcher::WriteCallMulti(void * const *addresses, std::size_t addressCount, const void * calleeAddress) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteCall(addresses[i], calleeAddress))
				return false;
		}

		return true;
	}

	//Int8 Writes
	bool Patcher::WriteInt8(void *addr, uint8_t value) const
	{
		return Write(addr, value);
	}

	bool Patcher::WriteInt8Multi(void * const *addresses, std::size_t addressCount, uint8_t value) const
	{
		return WriteMulti(addresses, addressCount, value);
	}

	//Int16 Writes
	bool Patcher::WriteInt16(void *addr, uint16_t value) const
	{
        return Write(addr, value);
	}

	bool Patcher::WriteInt16Multi(void * const *addresses, std::size_t addressCount, uint16_t value) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteInt16(addresses[i], value))
				return false;
		}

		return true;
	}

	//Int32 Writes
	bool Patcher::WriteInt32(void *addr, uint32_t value) const
	{
        return Write(addr, value);
	}

	bool Patcher::WriteInt32Multi(void * const *addresses, std::size_t addressCount, uint32_t value) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteInt32(addresses[i], value))
				return false;
		}

		return true;
	}

	//Int64 Writes
	bool Patcher::WriteInt64(void *addr, uint64_t value) const
	{
        return Write(addr, value);
	}

	bool Patcher::WriteInt64Multi(void * const *addresses, std::size_t addressCount, uint64_t value) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteInt64(addresses[i], value))
				return false;
		}

		return true;
	}

	//Pointer Writes
	bool Patcher::WritePointer(void *addr, void * value) const
	{
		return WriteInt32(addr, reinterpret_cast<uint32_t>(value));
	}

	bool Patcher::WritePointerMulti(void * const *addresses, std::size_t addressCount, const void * value) const
	{
		return WriteInt32Multi(addresses, addressCount, reinterpret_cast<uint32_t>(value));
	}

	//Array Writes
	bool Patcher::WriteArray(void *addr, const uint8_t *value, uint32_t length) const
	{
		if (length == 0)
			return true;

		return PatchManager::ApplyPatch(value, reinterpret_cast<uint8_t *>(addr), length, owner_);
	}

	bool Patcher::WriteArrayMulti(void * const *addresses, std::size_t addressCount, const uint8_t *value, uint32_t length) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteArray(addresses[i], value, length))
				return false;
		}

		return true;
	}

    //NOP Writes
	bool Patcher::WriteNOPs(void *addr, uint32_t length) const
	{
		if (length == 0)
			return true;

		std::vector<uint8_t> nopArray(length);
		memset(&(nopArray[0]), 0x90, length);
		return PatchManager::ApplyPatch(&nopArray[0], reinterpret_cast<uint8_t *>(addr), nopArray.size(), owner_);
	}

	bool Patcher::WriteNOPsMulti(void * const *addresses, std::size_t addressCount, uint32_t length) const
	{
		for (std::size_t i = 0; i < addressCount; ++i)
		{
			if (!WriteNOPs(addresses[i], length))
				return false;
		}

		return true;
	}
}