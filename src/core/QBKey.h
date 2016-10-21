#pragma once
#include "GH3Plus.h"
#include <stdint.h>

namespace GH3P
{
	/// A crc32 hash that is often used as a key in the guitar hero engine.
	struct GH3P_API QbKey final
	{
	private:
		/// The internal representation of the QbKey.
		uint32_t _key;

	public:
		/// Initialized a QbKey with a value of 0
		QbKey();

		/// Initializes a QbKey using an integer
		QbKey(uint32_t);

		/// Initializes a QbKey using a crc32 hash of the input string
		QbKey(const char *);

		/// Allow integer conversion
		inline operator uint32_t() const { return _key; }


		QbKey& operator=(const uint32_t);

		//Comparison Operators
		bool operator<(const QbKey &rhs) const;
		bool operator<=(const QbKey &rhs) const;
		bool operator>(const QbKey &rhs) const;
		bool operator>=(const QbKey &rhs) const;
		bool operator==(const QbKey &rhs) const;
		bool operator!=(const QbKey &rhs) const;
		
		//Mutators
		/// Chains a string to the end of the QbKey
		QbKey& operator+=(const char *rhs);
		

	};

	/// Chains a string to the end of a QbKey
	QbKey operator+(const QbKey &lhs, const char *rhs);
}