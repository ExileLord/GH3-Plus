#include "Patcher.h"
#include "PatchManager.h"

#include <vector>

namespace GH3P
{

	//Constructors and destructors
    Patcher::Patcher(GH3::QbKey owner) : _owner(owner) {}

	Patcher::~Patcher()
    {
		RemoveAllChanges();
    }

	bool Patcher::RemoveAllChanges()
	{
		return PatchManager::RemovePatches(_owner);
	}

    /// Templated function which writes a long jump variant at the specified address, the templated instruction can be used to
    /// specify the type of jump, that is a JMP, CALL, JZ, JG, etc.
	template<uint8_t instruction>
	bool WriteJmpVariant(void *addr, const void * jumpDestination, uint32_t owner)
	{
		uint8_t callBuffer[5];
		uint32_t jmpSize = reinterpret_cast<const uint8_t *>(jumpDestination) - reinterpret_cast<uint8_t *>(addr) - 5;

		callBuffer[0] = instruction;
		memcpy(&callBuffer[1], &jmpSize, sizeof(jmpSize));

		return PatchManager::ApplyPatch(callBuffer, reinterpret_cast<uint8_t *>(addr), 5, owner);
	}

	//Jumps
	bool Patcher::WriteJmp(void *addr, const void *jumpDestination)
	{
		return WriteJmpVariant<0xE9>(addr, jumpDestination, _owner); //0xE9 is the x86 JMP instruction
	}

	bool Patcher::WriteJmpMulti(void * const *addresses, std::size_t addressCount, const void *jumpDestination)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteJmp(addresses[i], jumpDestination))
				return false;
		}

		return true;
	}

	//Calls
	bool Patcher::WriteCall(void *addr, const void * calleeAddress)
	{
		return WriteJmpVariant<0xE8>(addr, calleeAddress, _owner); //0xE8 is the x86 CALL instruction
	}

	bool Patcher::WriteCallMulti(void * const *addresses, std::size_t addressCount, const void * calleeAddress)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteCall(addresses[i], calleeAddress))
				return false;
		}

		return true;
	}

	//Int8 Writes
	bool Patcher::WriteInt8(void *addr, uint8_t value)
	{
		return Write(addr, value);
	}

	bool Patcher::WriteInt8Multi(void * const *addresses, std::size_t addressCount, uint8_t value)
	{
		return WriteMulti(addresses, addressCount, value);
	}

	//Int16 Writes
	bool Patcher::WriteInt16(void *addr, uint16_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 2, _owner);
	}

	bool Patcher::WriteInt16Multi(void * const *addresses, std::size_t addressCount, uint16_t value)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteInt16(addresses[i], value))
				return false;
		}

		return true;
	}

	//Int32 Writes
	bool Patcher::WriteInt32(void *addr, uint32_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 4, _owner);
	}

	bool Patcher::WriteInt32Multi(void * const *addresses, std::size_t addressCount, uint32_t value)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteInt32(addresses[i], value))
				return false;
		}

		return true;
	}

	//Int64 Writes
	bool Patcher::WriteInt64(void *addr, uint64_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 8, _owner);
	}

	bool Patcher::WriteInt64Multi(void * const *addresses, std::size_t addressCount, uint64_t value)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteInt64(addresses[i], value))
				return false;
		}

		return true;
	}

	//Pointer Writes
	bool Patcher::WritePointer(void *addr, void * value)
	{
		return WriteInt32(addr, reinterpret_cast<uint32_t>(value));
	}

	bool Patcher::WritePointerMulti(void * const *addresses, std::size_t addressCount, const void * value)
	{
		return WriteInt32Multi(addresses, addressCount, reinterpret_cast<uint32_t>(value));
	}

	//Array Writes
	bool Patcher::WriteArray(void *addr, const uint8_t *value, uint32_t length)
	{
		if (length == 0)
			return true;

		return PatchManager::ApplyPatch(value, reinterpret_cast<uint8_t *>(addr), length, _owner);
	}

	bool Patcher::WriteArrayMulti(void * const *addresses, std::size_t addressCount, const uint8_t *value, uint32_t length)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteArray(addresses[i], value, length))
				return false;
		}

		return true;
	}

	template <typename Container>
	bool Patcher::WriteArrayMulti(const Container &addresses, const uint8_t *value, uint32_t length)
	{
		for (void *address : addresses)
		{
			if (!WriteArray(address, value))
				return false;
		}

		return true;
	}

	bool Patcher::WriteNOPs(void *addr, uint32_t length)
	{
		if (length == 0)
			return true;

		std::vector<uint8_t> nopArray(length);
		memset(&(nopArray[0]), 0x90, length);
		return PatchManager::ApplyPatch(&nopArray[0], reinterpret_cast<uint8_t *>(addr), nopArray.size(), _owner);
	}

	bool Patcher::WriteNOPsMulti(void * const *addresses, std::size_t addressCount, uint32_t length)
	{
		for (std::size_t i=0; i < addressCount; ++i)
		{
			if (!WriteNOPs(addresses[i], length))
				return false;
		}

		return true;
	}
}