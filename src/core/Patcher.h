#pragma once
#include "GH3Plus.h"
#include "core\PatchManager.h"
#include "gh3\QbKey.h"
#include <stdint.h>
#include <vector>


namespace GH3P
{

	/// An instance of this class can write to the process's protected address space and revert its changes when it is destroyed.
    class GH3P_API Patcher
    {
    private:
        GH3::QbKey owner_;
        Patcher() = delete;

    public:
        /// Creates an instance of the patcher with specified owner ID.
        /// The owner ID should be unique between plugins otherwise GH3+ will not be able to differentiate which plugin owns a set of patches.
        Patcher(GH3::QbKey owner);

        /// Destroys patcher instance, reverting all of its changes.
        ~Patcher();

        /// Reverts all changes made with this patcher instance.
        bool RemoveAllChanges();

        /// Writes an arbitrary type at the given address. The amount of bytes overwritten will be equal to the size of the type.
        template<typename T>
        bool Write(void *addr, T value) const
        {
            return PatchManager::ApplyPatch(reinterpret_cast<const uint8_t *>(&value), reinterpret_cast<uint8_t *>(addr), sizeof(T), owner_);
        }

        /// Writes an arbitrary type at the given addresses. The amount of bytes overwritten will be equal to the size of the type.
        template<typename T>
        bool WriteMulti(void * const *addresses, std::size_t addressCount, T value) const
        {
            for (std::size_t i = 0; i < addressCount; ++i)
            {
                if (!Write(addresses[i], value))
                    return false;
            }

            return true;
        }

        /// Writes an arbitrary type at the given addresses. The amount of bytes overwritten will be equal to the size of the type.
        template<typename Container, typename T>
        bool WriteMulti(const Container &addresses, T value) const
        {
            for (void *address : addresses)
            {
                if (!Write(address, value))
                    return false;
            }

            return true;
        }

        template<uint8_t instruction>
        bool WriteJmpVariant(void *addr, const void *jumpDestination) const
        {
            uint8_t buffer[5];
            uint32_t jmpSize = reinterpret_cast<const uint8_t *>(jumpDestination) - reinterpret_cast<uint8_t *>(addr) - 5;

            buffer[0] = instruction;
            memcpy(&buffer[1], &jmpSize, sizeof(jmpSize));

            return PatchManager::ApplyPatch(buffer, reinterpret_cast<uint8_t *>(addr), 5, owner_);
        }

        template<uint8_t instruction>
        bool WriteJmpVariantMulti(void *addr, const void *jumpDestination) const
        {
            for (std::size_t i = 0; i < addressCount; ++i)
            {
                if (!WriteJmpVariant<instruction>(addresses[i], jumpDestination))
                    return false;
            }

            return true;
        }

        template<typename Container, uint8_t instruction>
        bool WriteJmpVariantMulti(const Container &addresses, const void *jumpDestination) const
        {
            for (void *address : addresses)
            {
                if (!WriteJmpVariant<instruction>(address, jumpDestination))
                    return false;
            }

            return true;
        }

		/// Writes an unconditional jump instruction at the given address to the given destination. Five bytes will be overwritten.
        bool WriteJmp(void *addr, const void *jumpDestination) const;

		/// Writes an unconditional jump instruction at each address to the given destination. Five bytes will be overwritten at each address.
        bool WriteJmpMulti(void * const *addresses, std::size_t addressCount, const void *jumpDestination) const;

		template<typename Container>
        bool WriteJmpMulti(const Container &addresses, const void *jumpDestination) const
        {
            return WriteJmpVariantMulti<Container, 0xE9>(addresses, jumpDestination); //0xE9 is the x86 JMP instruction
        }

		/// Writes a call instruction at the given address to the given destination. Five bytes will be overwritten.
		bool WriteCall(void *addr, const void *calleeAddress) const;

		/// Writes a call instruction at each address to the given destination. Five bytes will be overwritten at each address.
		bool WriteCallMulti(void * const *addresses, std::size_t addressCount, const void *calleeAddress) const;

        template <typename Container>
        bool WriteCallMulti(const Container &addresses, const void *calleeAddress) const
        {
            return WriteJmpVariantMulti<Container, 0xE8>(addresses, calleeAddress); //0xE8 is the x86 CALL instruction
        }

		/// Writes an 8 bit integer at the given address. One byte will be overwritten.
		bool WriteInt8(void *addr, uint8_t value) const;

		/// Writes a 8 bit integer at the given addresses. One byte will be overwritten at each address.
		bool WriteInt8Multi(void * const *addresses, std::size_t addressCount, uint8_t value) const;

		template<typename Container>
		bool WriteInt8Multi(const Container &addresses, uint8_t value) const
		{
			return WriteMulti(addresses, value);
		}

		/// Writes a 16 bit integer at the given address. Two bytes will be overwritten.
		bool WriteInt16(void *addr, uint16_t value) const;

		/// Writes a 16 bit integer at the given addresses. Two bytes will be overwritten at each address.
		bool WriteInt16Multi(void * const *addresses, std::size_t addressCount, uint16_t value) const;

		template<typename Container>
		bool WriteInt16Multi(const Container &addresses, uint16_t value) const
		{
			return WriteMulti(addresses, value);
		}

		/// Writes a 32 bit integer at the given address. Four bytes will be overwritten.
		bool WriteInt32(void *addr, uint32_t value) const;

		/// Writes a 32 bit integer at the given addresses. Four bytes will be overwritten at each address.
		bool WriteInt32Multi(void * const *addresses, std::size_t addressCount, uint32_t value) const;

		template<typename Container>
		bool WriteInt32Multi(const Container &addresses, uint32_t value) const
		{
			return WriteMulti(addresses, value);
		}

		/// Writes a 64 bit integer at the given address. Two bytes will be overwritten.
		bool WriteInt64(void *addr, uint64_t value) const;

		/// Writes a 64 bit integer at the given addresses. Eight bytes will be overwritten at each address.
		bool WriteInt64Multi(void * const *addresses, std::size_t addressCount, uint64_t value) const;

		template<typename Container>
		bool WriteInt64Multi(const Container &addresses, uint64_t value) const
		{
			return WriteMulti(addresses, value);
		}

		/// Writes a 32 bit pointer at the given address. Four bytes will be overwritten.
		bool WritePointer(void *addr, void * value) const;

		/// Writes a 32 bit pointer at the given addresses. Four bytes will be overwritten at each address.
		bool WritePointerMulti(void * const *addresses, std::size_t addressCount, const void * value) const;

		template<typename Container>
		bool WritePointerMulti(const Container &addresses, const void * value) const
		{
			return WriteMulti(addresses, value);
		}

		/// Writes the contents of an array of bytes at the given address.
		bool WriteArray(void *addr, const uint8_t *value, uint32_t length) const;

		/// Writes the contents of an array of bytes at the given addresses.
		bool WriteArrayMulti(void * const *addresses, std::size_t addressCount, const uint8_t *value, uint32_t length) const;

		template<typename Container>
        bool WriteArrayMulti(const Container &addresses, const uint8_t *value, uint32_t length) const
        {
            for (void *address : addresses)
            {
                if (!WriteArray(address, value))
                    return false;
            }
        }

		/// Writes NOP instructions at a given address.
		bool WriteNOPs(void *addr, uint32_t length) const;

		/// Writes NOP instructions at the given addresses.
		bool WriteNOPsMulti(void * const *addresses, std::size_t addressCount, uint32_t length) const;

        template<typename Container>
        bool WriteNOPsMulti(const Container &addresses, uint32_t length) const
        {
            for (void *address : addresses)
            {
                if (!WriteNOPs(address, length))
                    return false;
            }
        }
    };
}