#include "Patcher.h"
#include "PatchManager.h"
#include <map>


namespace GH3P
{
	Patcher::Patcher(QBKey owner) : _owner(owner)
	{
	}

	Patcher::~Patcher()
    {
		PatchManager::RemovePatches(_owner);
    }

	bool Patcher::WriteJmp(void * addr, void * jumpDestination)
	{
		uint8_t jumpBuffer[5];
		uint32_t jmpSize = reinterpret_cast<uint8_t *>(jumpDestination) - reinterpret_cast<uint8_t *>(addr) - 5;

		jumpBuffer[0] = 0xE9; //unconditional jump instruction
		memcpy(&jumpBuffer[1], &jmpSize, sizeof(jmpSize));

		return PatchManager::ApplyPatch(jumpBuffer, reinterpret_cast<uint8_t *>(addr), 5, _owner);
	}

	bool Patcher::WriteInt8(void * addr, uint8_t value)
	{
		return PatchManager::ApplyPatch(&value, reinterpret_cast<uint8_t *>(addr), 1, _owner);
	}

	bool Patcher::WriteInt16(void * addr, uint16_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 2, _owner);
	}

	bool Patcher::WriteInt32(void * addr, uint32_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 4, _owner);
	}

	bool Patcher::WriteInt64(void * addr, uint64_t value)
	{
		return PatchManager::ApplyPatch(reinterpret_cast<uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), 8, _owner);
	}

	bool Patcher::WriteArray(void * addr, uint8_t *value, uint32_t length)
	{
		return PatchManager::ApplyPatch(value, reinterpret_cast<uint8_t *>(addr), length, _owner);
	}

	bool Patcher::WriteNOPs(void * addr, uint32_t length)
	{
		std::vector<uint8_t> nopArray(length);
		memset(&(nopArray[0]), 0x90, length);
		return PatchManager::ApplyPatch(nopArray, reinterpret_cast<uint8_t *>(addr), _owner);
	}
}