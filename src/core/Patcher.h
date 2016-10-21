#pragma once
#include "GH3Plus.h"
#include <stdint.h>
#include "QbKey.h"

namespace GH3P
{

	/// An instance of this class can write to the process's protected address space and revert its changes when it is destroyed.
    class GH3P_API Patcher
    {
    private:
		QbKey _owner;
		Patcher() = delete;
    public:
		/// Creates an instance of the patcher with specified owner ID.
		/// The owner ID should be unique between plugins otherwise GH3+ will not be able to differentiate which plugin owns a set of patches.
		Patcher(QbKey owner);

		/// Destroys patcher instance, reverting all of its changes.
        ~Patcher();

		/// Reverts all changes made with this instance.
		bool RemoveAllChanges();



		/// Writes an unconditional jump instruction at the given address to the given destination. Five bytes will be overwritten.
		bool WriteJmp   (void *addr, void *jumpDestination);

		/// Writes an 8 bit integer at the given address. One byte will be overwritten.
		bool WriteInt8  (void *addr, uint8_t value);

		/// Writes a 16 bit integer at the given address. Two bytes will be overwritten.
		bool WriteInt16 (void *addr, uint16_t value);

		/// Writes a 32 bit integer at the given address. Two bytes will be overwritten.
		bool WriteInt32 (void *addr, uint32_t value);

		/// Writes a 64 bit integer at the given address. Two bytes will be overwritten.
		bool WriteInt64 (void *addr, uint64_t value);

		/// Writes the contents of an array of bytes at the given address.
		bool WriteArray (void *addr, uint8_t *value, uint32_t length);

		/// Writes NOP instructions at a given address.
		bool WriteNOPs  (void *addr, uint32_t length);
    };
}