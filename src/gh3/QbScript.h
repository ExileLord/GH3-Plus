#pragma once

#include "core\GH3Plus.h"
#include "QbStruct.h"
#include "QbKey.h"
#include <stdint.h>

namespace GH3
{
    /// Neversoft's script class. Most of the contents of the script class are unknown. Also known as CScript
	struct GH3P_API QbScript
	{
		uint8_t gap0[16];
		uint8_t *instructionPointer;
		QbStruct *qbStruct14;
		uint32_t dword18;
		QbStruct *qbStruct1C;
		uint8_t gap20[16];
		uint8_t gap30[16];
		uint8_t gap40[16];
		uint8_t gap50[16];
		uint8_t gap60[16];
		uint8_t gap70[16];
		uint8_t gap80[16];
		uint8_t gap90[16];
		uint32_t dwordA0;
		uint32_t dwordA4;
		uint32_t unkStructPtrA8;
		uint32_t dwordAC;
		uint32_t unkB0;
		uint32_t dwordB4;
		uint32_t dwordB8;
		uint8_t unkBC;
		uint8_t unkBD;
		uint8_t unkBE;
		uint8_t unkBF;
		uint8_t *nextIP;
		uint8_t unkC4;
		uint8_t unkC5;
		uint8_t unkC6;
		uint8_t unkC7;
		uint32_t dwordC8;
		uint32_t dwordCC;
		uint32_t dwordD0;
		uint32_t dwordD4;
		QbKey type;
	};

	///used for struct items?
	static void (__cdecl * const SpawnScript)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void (__cdecl *)(QbStruct *, QbScript *)>(0x00534100);
	static void (__cdecl * const SpawnScriptNow)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534140);
	static void (__cdecl * const SpawnScriptLater)(QbStruct *aStruct, QbScript *parentScript) = reinterpret_cast<void(__cdecl *)(QbStruct *, QbScript *)>(0x00534160);
}

