#pragma once
#include <stdint.h>
#include "gh3\CRefCounted.h"
#include "gh3\CAliveRefCounted.h"
#include "gh3\QbKey.h"
#include "gh3\QbStruct.h"
#include "gh3\List.h"

namespace GH3
{
    /// Object. Base class of a large amount of classes in the Neversoft engine.
	class GH3P_API CObject : CRefCounted
	{
	public:
		enum CObjectFlags
		{
			UnkFlag1 = 0x1,
			UnkFlag2 = 0x2,
			UnkFlag3 = 0x4,
			UnkFlag4 = 0x8,
			UnkFlag5 = 0x10,
			UnkFlag6 = 0x20,
			UnkFlag7 = 0x40,
			UnkFlag8 = 0x80,
			UnkFlag9 = 0x100,
			UnkFlag10 = 0x200,
			UnkAnimationFlag11 = 0x400,
			UnkFlag12 = 0x800,
			UnkFlag13 = 0x1000,
			OddScaleFlag = 0x2000,
			UnkFlag15 = 0x4000,
			UnkFlag16 = 0x8000,
			UnkFlag17 = 0x10000,
			UnkFlag18 = 0x20000,
			UnkFlag19 = 0x40000,
			Invisible1 = 0x80000,
			UnkFlag21 = 0x100000,
			UnkFlag22 = 0x200000,
			Invisible2 = 0x400000,
			UnkFlag24 = 0x800000,
			MessesUpAnglesFlag = 0x1000000,
			MirrorVertically = 0x2000000,
			MirrorHorizontally = 0x4000000,
			UnkFlag28 = 0x8000000,
			UnkFlag29 = 0x10000000,
			Immortal = 0x20000000,
			UnkFlag31 = 0x40000000,
			UnkFlag32 = 0x80000000,
		};

		struct VFTable
		{
			CObject *(__thiscall *dtor)(CObject *this_, bool onHeap);
			int (__thiscall *method1)(CObject *this_);
			int (__thiscall *method2)(CObject *this_, QbStruct *);
			int (__thiscall *method3)(CObject *this_);
			int (__thiscall *method4)(CObject *this_);
			int (__thiscall *method5)(CObject *this_);
			int (__thiscall *method6)(CObject *this_);
			int (__thiscall *method7)(CObject *this_);
			int (__thiscall *method8)(CObject *this_);
		};

		CObjectFlags flags;
		uint32_t *unkClass_C;
		uint32_t field_10;
		uint32_t field_14;
		uint32_t field_18;
		QbKey elementType;
		uint8_t field_20;
		uint32_t field_24;
		QbStruct *unkStruct;
		QbKey name;
		uint32_t field_30;
		uint32_t field_34;
		Lst::Node<CObject> node;
		uint32_t field_4C;
		uint32_t field_50;
		uint32_t field_54;
		CAliveRefCounted crefcounted58;
	};
}