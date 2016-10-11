#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"


#define PUSH_ALL_BUT_EDX \
__asm push esp \
__asm push ebp \
__asm push eax \
__asm push ebx \
__asm push ecx \
__asm push esi \
__asm push edi \

#define POP_ALL_BUT_EDX \
__asm pop edi \
__asm pop esi \
__asm pop ecx \
__asm pop ebx \
__asm pop eax \
__asm pop ebp \
__asm pop esp \

#define SP_TAP_NOTE_GEM_INDEX 3




//Types

enum class EHopoFlag : uint32_t
{
	STRUMMED = 0,
	HOPO = 1,
	TAPPING = 2
};

struct Note
{
	uint32_t vTable;
	uint32_t fret[5];
	EHopoFlag hopoFlag;
};


//Detour addresses
static void * const cave1DetourAddress =			(void *)0x0041D22E;
static void * const guitarInputLogicDetour1 =		(void *)0x00431A79;
static void * const guitarInputLogicDetour2 =		(void *)0x00431B35;
static void * const guitarInputLogicDetour2b =		(void *)0x00431906;
static void * const chordHitCheckDetour =			(void *)0x00431EA6;
static void * const noteHitEndDetour =				(void *)0x004320AD;
static void * const noteHitEndDetour2 =				(void *)0x0043214C;
static void * const setStarGemAppearanceDetour =	(void *)0x004328C2;
static void * const setNormalGemAppearanceDetour =	(void *)0x0043293A;
static void * const setSpGemAppearanceDetour =		(void *)0x0043289E;

//GH3 functions
static const void * const ProbablyGetDWord = (void *)0x0040A9B0;

//GH3 globals
static uint32_t *g_hitFrets = (uint32_t *)ADDR_hitFrets;
static uint32_t **g_crazyPointerChain = (uint32_t **)ADDR_CrazyPointerChain;
static uint32_t *g_currentNote = (uint32_t *)ADDR_currentNote;
static uint32_t *g_pressedFrets = (uint32_t *)ADDR_pressedFrets;
static uint32_t *g_hitNoNotes_bool = (uint32_t *)ADDR_hitNoNotes_bool;

//Hack globals
static uint32_t g_canTap[2] = { 0 };
static uint32_t g_canTapLow[2] = { 0 };
static uint32_t g_lastHit[2] = { 0 };

//Pre-declarations
uint32_t *LoadCurrentNoteStruct(uint32_t pIdx);
uint32_t getHighFret(uint32_t fretMask);
uint32_t getAnchoredMask(uint32_t fretMask);
bool hitHopoChord(uint32_t fretsHeld, uint32_t noteMask);
bool hitHopoChordHigh(uint32_t fretsHeld, uint32_t noteMask);
bool hitHopoChordLow(uint32_t fretsHeld, uint32_t noteMask);
uint32_t __stdcall loadLastHit(uint32_t pIdx);
void __stdcall setCanTapState(uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx);
int __stdcall chordHitCheck(EHopoFlag hopoFlag, uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx);
void __stdcall noteHitEnd(uint32_t pIdx);

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);


////////////////
// ~~~~~~~~~~ //
////////////////



// Injected into the code loading an individual note
__declspec( naked ) void loadTappingFlagNaked()
{
	static const int frameOffset = 0x48 + 0x4*8; //push an addition 4 for each offset register

	//Needed stack variables of GuitarInputFrame
	static const int arg_C = 0x10 + frameOffset;
	static const int obj = 0x04 + frameOffset;
	static const int hopoRaw = -0x28 + frameOffset;

	static const unsigned int returnAddress = 0x0041D239;

	//Too fucking lazy to convert this to C
	//It's just loading the tapping flag and setting hopo flag to 2 if it has one
	_asm
	{
	   pushad

	   mov      eax, [esp + 0x78] //[esp + arg_C]
	   mov      edx, [eax + 4]
	   mov      eax, [eax + 8]
	   mov      edi, eax
	   mov      ecx, eax
	   cmp      edx, 1
	   jz       branch_a
	   mov      edi, [eax + esi * 4 + 4]
	   mov      ecx, [eax + esi * 4 + 8]
				
branch_a:       
	   sar      ecx, 6
	   and      ecx, 1
	   cmp      ecx, 1
	   jnz      branch_b
	   mov      edi, 2
	   mov      [esp + 0x40], edi //[esp + hopoRaw]

branch_b:

	   popad;

	   mov     eax, KEY_PLAYER ; //Existing code we pushed out of the way to jump to this cave
	   push    eax
	   call    ProbablyGetDWord

	   jmp     returnAddress   ; //Return to normal execution

	};
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void loadLastHitNaked()
{
	static const uint32_t returnAddress = 0x00431A80;

	_asm
	{
		PUSH_ALL_BUT_EDX

		push ecx; //pIdx
		call loadLastHit;
		mov edx, eax;
		
		POP_ALL_BUT_EDX

		jmp returnAddress;
	};
}

uint32_t __stdcall loadLastHit(uint32_t pIdx)
{
	EHopoFlag hopoFlag;

	uint32_t *noteStruct = LoadCurrentNoteStruct(pIdx);
	uint32_t fretCount = 0;
	bool isChord = false;

	//Get the hopo flag out of the note structure
	if (noteStruct[1] == 1)
	{
		//1D Case (this probably doesn't ever get hit because it seems like insanity)
		EHopoFlag *flagPtr = reinterpret_cast<EHopoFlag*>(noteStruct);
		hopoFlag = flagPtr[2];
	}
	else
	{
		//2D Case
		Note &note = *reinterpret_cast<Note *>(noteStruct[2]);
		hopoFlag = note.hopoFlag;
		for (int i = 0; i < 5; ++i)
		{
			if (note.fret[i] == 1)
				++fretCount;
		}
		if (fretCount > 1)
			isChord = true;
	}

	//Accounting for chords here fixes an edge case that would be painful to fix in later code
	if (hopoFlag == EHopoFlag::HOPO && g_canTapLow[pIdx] && g_hitFrets[pIdx] != 0x00000 && isChord) //tapping note
	{
		return 0x11111111;
	}

	if (hopoFlag == EHopoFlag::TAPPING && (g_canTap[pIdx] || (g_canTapLow[pIdx] && isChord))) //tapping note
	{
		return 0x11111111;
	}
	else
	{
		return g_hitFrets[pIdx];
	}

}



uint32_t *LoadCurrentNoteStruct(uint32_t pIdx)
{

	uint32_t *playerChain = g_crazyPointerChain[pIdx];
	if (playerChain[1] == 1)
	{
		uint32_t **playerAltChain = reinterpret_cast<uint32_t **>(playerChain);
		return playerAltChain[2];
	}
	else
	{
		uint32_t ***playerAltChain = reinterpret_cast<uint32_t ***>(playerChain);
		return playerAltChain[2][g_currentNote[pIdx]];
	}
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void setCanTapStateNaked()
{
	static const uint32_t returnAddress = 0x00431B43;

	_asm
	{
		pushad;
		push ecx;                         //arg 3 pIdx
		push ebx;                         //arg 2 currFrets
		mov eax, [ADDR_pressedFrets + ecx * 4]; 
		push eax;                         //arg 1 prevFrets
		call setCanTapState;
		popad;

		mov eax, [ADDR_CrazyPointerChain + ecx * 4];
		mov [ADDR_pressedFrets + ecx * 4], ebx;
		jmp returnAddress;
	};
}

void __stdcall setCanTapState(uint32_t currFrets, uint32_t prevFrets, uint32_t pIdx)
{
	if (getHighFret(currFrets) != getHighFret(prevFrets))
		g_canTap[pIdx] = true;

	if (currFrets != prevFrets)
		g_canTapLow[pIdx] = true;
}

uint32_t getHighFret(uint32_t fretMask)
{
	if (fretMask & 0x00001)
		return  0x00001;

	if (fretMask & 0x00010)
		return  0x00010;

	if (fretMask & 0x00100)
		return  0x00100;

	if (fretMask & 0x01000)
		return  0x01000;

	if (fretMask & 0x10000)
		return  0x10000;

	return 0x00000;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void chordHitCheckNaked()
{

	static const uint32_t CHORD_CHECK_FAILED = 0x00431EE3;
	static const uint32_t CHORD_CHECK_PASSED = 0x00431EBA;
	static const uint32_t NOTE_HIT_HOPO = 0x00431F50;

	static const int frameOffset = 0x198;

	static const int isHopo2 = -0x164 + frameOffset; //0x34
	static const int pIdx = -0x170 + frameOffset; //0x28
	static const int pIdx9 = -0x188 + frameOffset; //0x10

	__asm
	{
		mov ecx, [esp + 0x10];
		push ecx                     // pIdx
		push ebx;                    // note mask
		push [ADDR_pressedFrets + ecx * 4] // pressed frets
		push[esp + 0x40];        // hopo flag
		call chordHitCheck;
		cmp eax, 1;
		je L_NOTE_HIT_HOPO;

	//L_CHORD_FIRST_CHECK:; //Normal chord. Not HOPO (or hopo failed)
		cmp		byte ptr[esp + 17h], 0;  //var_184 + 3. I don't know what this is
		jnz		L_CHORD_CHECK_PASSED
		mov		ecx, [esp + 0x10]; //pIdx9 
		cmp     [ADDR_hitNoNotes_bool + ecx * 4], edi; 
		jnz		L_CHORD_CHECK_FAILED
		jmp		CHORD_CHECK_PASSED

		//Because MASM doesn't support conditional direct jumps
	L_CHORD_CHECK_FAILED:
		jmp		CHORD_CHECK_FAILED;
	L_CHORD_CHECK_PASSED:
		jmp		CHORD_CHECK_PASSED;
	L_NOTE_HIT_HOPO:
		jmp		NOTE_HIT_HOPO;
	}
}

int __stdcall chordHitCheck(EHopoFlag hopoFlag, uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx)
{
	if (hopoFlag == EHopoFlag::STRUMMED)
		return 0;

	if (g_canTap[pIdx] && hitHopoChordHigh(fretsHeld, noteMask))
		return 1;

	if (g_canTapLow[pIdx] && hitHopoChordLow(fretsHeld, noteMask))
		return 1;

	return 0;
}

bool hitHopoChordHigh(uint32_t fretsHeld, uint32_t noteMask)
{
	if ((fretsHeld & noteMask) != noteMask)
		return false;

	if ((fretsHeld & getAnchoredMask(noteMask)) != fretsHeld)
		return false;

	return true;
}

bool hitHopoChordLow(uint32_t fretsHeld, uint32_t noteMask)
{
	return (fretsHeld == noteMask);
}


uint32_t getAnchoredMask(uint32_t fretMask)
{
	if (fretMask >= 0x10000)
		return fretMask;

	if (fretMask >= 0x01000)
		return fretMask | 0x10000;

	if (fretMask >= 0x00100)
		return fretMask | 0x11000;

	if (fretMask >= 0x00010)
		return fretMask | 0x11100;

	if (fretMask >= 0x00001)
		return fretMask | 0x11110;

	return 0x11111;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void noteHitEndNaked()
{
	const static uint32_t normalReturnAddress = 0x00432166;
	const static uint32_t debugReturnAddress = 0x00432153;

	__asm
	{
		pushad;
		mov eax, [esp + 30h]; //pIdx9 + 20h
		push eax;
		call noteHitEnd;
		popad;

		//original displaced code
		cmp dword ptr[esp + 2Ch], 0; //198h + input.field_0
		jz L_NOT_DEBUG;
		jmp debugReturnAddress;
	L_NOT_DEBUG:
		jmp normalReturnAddress;
	}
}

void __stdcall noteHitEnd(uint32_t pIdx)
{
	g_lastHit[pIdx] = g_pressedFrets[pIdx];
	g_canTap[pIdx] = 0;
	g_canTapLow[pIdx] = 0;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////
 
///////////////////////////////
// Primary gem loading logic //
///////////////////////////////
__declspec(naked) void makeStarTapGemsNaked()
{
	const static uint32_t tappingReturn = 0x00432901;

	const static uint32_t bossReturn = 0x004328ED;
	const static uint32_t normalReturn = 0x004328D7;
	
	//edx = 1
	//edi = gemMaterial2
	//eax = gemIndex
	//ebp = hammerOn flag
	// This is easier to do in asm since it's so simple. The bulk of this is moving the original logic around
	__asm
	{
		cmp ebp, edx; //cmp hopo flag vs 1
		jle L_NOT_TAPPING; //if 0 or 1 then it's strummed or hopo

		//tapping note
		mov edi, [ADDR_gemMatHammerBattle2 + eax * 4];
		jmp tappingReturn;

	L_NOT_TAPPING:
		push eax
		mov eax, ADDR_gameMode
		cmp dword ptr [eax], KEY_P2_BATTLE;
		je L_BOSS_BATTLE;
		mov eax, ADDR_bossBattle
		cmp dword ptr[eax], 0;
		jnz L_BOSS_BATTLE;

		pop eax
		jmp normalReturn;

	L_BOSS_BATTLE:
		pop eax;
		jmp bossReturn;
	}
}

__declspec(naked) void makeTapGemsNaked()
{
	const static uint32_t returnAddress = 0x0043294E;
	__asm
	{
		cmp     ebp, edx;
		jg      L_TAPPING_GEM;
		je      L_HAMMER_GEM;
		
		mov edi, [ADDR_gemMatNormal2 + eax * 4];
		jmp returnAddress;

	L_HAMMER_GEM:
		mov edi, [ADDR_gemMatHammer2 + eax * 4];
		jmp returnAddress;

	L_TAPPING_GEM:
		mov edi, [ADDR_gemMatBattle2 + eax * 4];
		jmp returnAddress;
	}
}

__declspec(naked) void makeSpTapGemsNaked()
{
	const static uint32_t returnAddress = 0x004328A5;

	//.data:00A17868 g_gemMatHammerSp2 dd 0  
	__asm
	{
		mov		edx, 2;
		cmp		ebp, edx;
		mov		edx, 1;
		jnz		NORMAL_NOTE;

	TAP_NOTE:     
		mov		eax, SP_TAP_NOTE_GEM_INDEX;
		mov     edi, [ADDR_gemMatBattle2 + eax * 4]; // g_gemMatHammerSp2[eax * 4]
		jmp		returnAddress;

	NORMAL_NOTE:
		mov     edi, [eax * 4 + 0x00A1787C]; // g_gemMatHammerSp2[eax * 4]
		jmp returnAddress;
	}
}








///////////////////////
// Gem mutator logic //
///////////////////////




static void * const gemMutationSPActivationBranch = (void *)0x004276AC; //redirect to gemMutationSPActivationBranchNaked
static void * const gemMutationSPActivationBranch_Hammer = (void *)0x004276B2;
static void * const gemMutationSPActivationBranch_NoHammer = (void *)0x004276D9;
static void * const gemMutationSPActivationBranch_Tap = (void *)0x004276CF;

// Fixes tap note mutation upon activating star power
_declspec(naked) void gemMutationSPActivationBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

		//Tap Notes
		mov		eax, SP_TAP_NOTE_GEM_INDEX;
		mov     eax, [ADDR_gemMatBattle2 + eax * 4]; // g_gemMatHammerSp2[eax * 4]
		mov     ecx, [ADDR_someStruct + esi * 4];	//this
		push    eax;	//textureKey
		jmp     gemMutationSPActivationBranch_Tap;

	NO_HAMMER:
		jmp		gemMutationSPActivationBranch_NoHammer;
	HAMMER:
		mov     ecx, [esp + 10h];				// [esp + 0F4h + keyDest]; this
		jmp		gemMutationSPActivationBranch_Hammer;
	}
}




static void * const gemMutationSPDeactivationBranch =				(void *)0x00427768; //redirect to gemMutationSPDeactivationBranchNaked
static void * const gemMutationSPDeactivationBranch_HammerOrTap =	(void *)0x0042776F;
static void * const gemMutationSPDeactivationBranch_NoHammer =		(void *)0x00427791;

// Fixes tap note mutation upon losing star power
_declspec(naked) void gemMutationSPDeactivationBranchNaked()
{
	__asm
	{	
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

		//Tap Notes
		mov     eax, KEY_BATTLE_STAR_MATERIAL;
		jmp     gemMutationSPDeactivationBranch_HammerOrTap;

	NO_HAMMER:
		jmp		gemMutationSPDeactivationBranch_NoHammer;
	HAMMER:
		mov     eax, KEY_GEM_HAMMER_MATERIAL
		jmp		gemMutationSPDeactivationBranch_HammerOrTap;
	}
}

static void * const gemMutationPhraseMissBranch =					(void *)0x00427B86; //redirect to gemMutationPhraseMissBranchNaked
static void * const gemMutationPhraseMissBranch_NoHammer =			(void *)0x00427BA0;
static void * const gemMutationPhraseMissBranch_HammerAndTap =		(void *)0x00427B8D;

// Fixes tap note mutation upon missing SP phrase
_declspec(naked) void gemMutationPhraseMissBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

		//Tap Notes
		mov     eax, KEY_BATTLE_STAR_MATERIAL;
		jmp     gemMutationPhraseMissBranch_HammerAndTap;

	NO_HAMMER:
		jmp		gemMutationPhraseMissBranch_NoHammer;
	HAMMER:
		mov     eax, KEY_GEM_HAMMER_MATERIAL;
		jmp		gemMutationPhraseMissBranch_HammerAndTap;
	}
}




static void * const gemMutationSPtoStarBranch =				(void *)0x00427D0E; //redirect to gemMutationSPtoStarBranchNaked
static void * const gemMutationSPtoStarBranch_Hammer =		(void *)0x00427D14;
static void * const gemMutationSPtoStarBranch_NoHammer =	(void *)0x00427D2F;
static void * const gemMutationSPtoStarBranch_Tap =			(void *)0x00427D19;

// Fixes tap note mutation upon star power running out with an SP phrase back into gems
_declspec(naked) void gemMutationSPtoStarBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

		//Tap Notes
		mov     ecx, [esp + 10h]; //this //[esp + 0F4h + keyDest]
		mov     eax, KEY_BATTLE_STAR_HAMMER_MATERIAL;
		jmp     gemMutationSPtoStarBranch_Tap;

	NO_HAMMER:
		jmp		gemMutationSPtoStarBranch_NoHammer;

	HAMMER:
		mov     ecx, [esp + 10h]; //this //[esp + 0F4h + keyDest]
		jmp		gemMutationSPtoStarBranch_Hammer;
	}
}


// 00427AAB
static void * const gemMutationStarOverlapBranch =			(void *)0x00427AAB; //redirect to gemMutationStarOverlapBranchNaked
static void * const gemMutationStarOverlapBranch_Exit =		(void *)0x00427646;

// Fixes tap note mutation upon star power overlapping (previously) star gems
_declspec(naked) void gemMutationStarOverlapBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

		//Tap Notes
		mov		eax, SP_TAP_NOTE_GEM_INDEX;
		mov     eax, [ADDR_gemMatBattle2 + eax * 4]; // g_gemMatHammerSp2[eax * 4]

	HAMMER:
		jmp		gemMutationStarOverlapBranch_Exit;
	}
}


void ApplyHack()
{
	g_patcher.WriteJmp(cave1DetourAddress, &loadTappingFlagNaked);

	g_patcher.WriteJmp(guitarInputLogicDetour1, &loadLastHitNaked);
	g_patcher.WriteJmp(guitarInputLogicDetour2, &setCanTapStateNaked);
	g_patcher.WriteJmp(chordHitCheckDetour, &chordHitCheckNaked);

	uint8_t *jnz_to_CHORD_CHECK_FAILED = (uint8_t *)(0x00431E75); // test before was cmp hopo vs 1
	g_patcher.WriteInt8(jnz_to_CHORD_CHECK_FAILED, 0x7C);

	g_patcher.WriteJmp(noteHitEndDetour2, &noteHitEndNaked);

	//Normal gem logic
	g_patcher.WriteJmp(setStarGemAppearanceDetour, &makeStarTapGemsNaked);
	g_patcher.WriteJmp(setNormalGemAppearanceDetour, &makeTapGemsNaked);
	g_patcher.WriteJmp(setSpGemAppearanceDetour, &makeSpTapGemsNaked);

	//Gem mutation logic
	g_patcher.WriteJmp(gemMutationSPActivationBranch, &gemMutationSPActivationBranchNaked);
	g_patcher.WriteJmp(gemMutationSPDeactivationBranch, &gemMutationSPDeactivationBranchNaked);
	g_patcher.WriteJmp(gemMutationPhraseMissBranch, &gemMutationPhraseMissBranchNaked);
	g_patcher.WriteJmp(gemMutationStarOverlapBranch, &gemMutationStarOverlapBranchNaked);
	g_patcher.WriteJmp(gemMutationSPtoStarBranch, &gemMutationSPtoStarBranchNaked);

}
