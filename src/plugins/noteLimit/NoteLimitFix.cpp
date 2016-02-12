// GH3Hack.cpp : Defines the exported functions for the DLL application.
//

#include "NoteLimitFix.h"
#include <GH3Plus.h>
#include <Windows.h>
#include <stdint.h>

const uint32_t MAX_NOTES = 0x100000; //?1,048,576?

const LPDWORD SIZEOP_NOTE_ALLOCATION = (LPDWORD)0x0041AA78;

const LPDWORD ADDROP_SUSTAINARRAY_1 = (LPDWORD)0x0041EE33;
const LPDWORD ADDROP_SUSTAINARRAY_2 =  (LPDWORD)0x00423CD4;
const LPDWORD ADDROP_SUSTAINARRAY_3 =  (LPDWORD)0x00423D02;
const LPDWORD ADDROP_FCARRAY =		   (LPDWORD)0x00423D14;
const LPDWORD ADDROP_NOTEOFFSETARRAY = (LPDWORD)0x00423D22;

float *fixedSustainArray = nullptr;
float *fixedFcArray = nullptr;
uint32_t *fixedOffsetArray = nullptr;


//Predeclarations



// Writes to the code segment of GH3, overwriting the operands responsible for creating or pointing
// towards the fixed 4000 size note arrays with its own allocated arrays
void FixNoteLimit()
{
	gh3p::ScopedUnprotect scopedUnprotect{};

	fixedSustainArray = new float[MAX_NOTES * gh3p::GH3_MAX_PLAYERS];
	fixedFcArray =      new float[MAX_NOTES * gh3p::GH3_MAX_PLAYERS];
	fixedOffsetArray =  new uint32_t[MAX_NOTES * gh3p::GH3_MAX_PLAYERS];

	// Fix dynamic allocations
	*SIZEOP_NOTE_ALLOCATION = MAX_NOTES;

	// Fix static references
	*ADDROP_SUSTAINARRAY_1 =  (DWORD)fixedSustainArray;
	*ADDROP_SUSTAINARRAY_2 =  (DWORD)fixedSustainArray;
	*ADDROP_SUSTAINARRAY_3 =  (DWORD)fixedSustainArray;
	*ADDROP_FCARRAY =         (DWORD)fixedFcArray;
	*ADDROP_NOTEOFFSETARRAY = (DWORD)fixedOffsetArray;

}

