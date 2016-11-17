#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"




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
		mov     eax, [g_gemMatBattle + eax * 4]; // g_gemMatHammerSp2[eax * 4]
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




static void * const gemMutationSPDeactivationBranch = (void *)0x00427768; //redirect to gemMutationSPDeactivationBranchNaked
static void * const gemMutationSPDeactivationBranch_HammerOrTap = (void *)0x0042776F;
static void * const gemMutationSPDeactivationBranch_NoHammer = (void *)0x00427791;

// Fixes tap note mutation upon losing star power
_declspec(naked) void gemMutationSPDeactivationBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp		[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
		jz		HAMMER;

	//TAP:
		mov     eax, KEY_BATTLE_STAR_MATERIAL;
		jmp     gemMutationSPDeactivationBranch_HammerOrTap;

	NO_HAMMER:
		jmp		gemMutationSPDeactivationBranch_NoHammer;

	HAMMER:
		mov     eax, KEY_GEM_HAMMER_MATERIAL;
		jmp		gemMutationSPDeactivationBranch_HammerOrTap;
	}
}

static void * const gemMutationPhraseMissBranch = (void *)0x00427B86; //redirect to gemMutationPhraseMissBranchNaked
static void * const gemMutationPhraseMissBranch_NoHammer = (void *)0x00427BA0;
static void * const gemMutationPhraseMissBranch_HammerAndTap = (void *)0x00427B8D;

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




static void * const gemMutationSPtoStarBranch = (void *)0x00427D0E; //redirect to gemMutationSPtoStarBranchNaked
static void * const gemMutationSPtoStarBranch_Hammer = (void *)0x00427D14;
static void * const gemMutationSPtoStarBranch_NoHammer = (void *)0x00427D2F;
static void * const gemMutationSPtoStarBranch_Tap = (void *)0x00427D19;

// Fixes tap note mutation upon star power running out with an SP phrase back into gems
_declspec(naked) void gemMutationSPtoStarBranchNaked()
{
	__asm
	{
		// Previous comparison was cmp		[esp + 14h], 0
		jz		NO_HAMMER;
		cmp[esp + 14h], 1;  //[esp + 0F4h + hammerOn]
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
static void * const gemMutationStarOverlapBranch = (void *)0x00427AAB; //redirect to gemMutationStarOverlapBranchNaked
static void * const gemMutationStarOverlapBranch_Exit = (void *)0x00427646;

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
		mov     eax, [g_gemMatBattle + eax * 4]; // g_gemMatHammerSp2[eax * 4]

	HAMMER:
		jmp		gemMutationStarOverlapBranch_Exit;
	}
}

bool TryApplyGemMutationPatches()
{
	return  (g_patcher.WriteJmp(gemMutationSPActivationBranch, &gemMutationSPActivationBranchNaked) &&
			g_patcher.WriteJmp(gemMutationSPDeactivationBranch, &gemMutationSPDeactivationBranchNaked) &&
			g_patcher.WriteJmp(gemMutationPhraseMissBranch, &gemMutationPhraseMissBranchNaked) &&
			g_patcher.WriteJmp(gemMutationStarOverlapBranch, &gemMutationStarOverlapBranchNaked) &&
			g_patcher.WriteJmp(gemMutationSPtoStarBranch, &gemMutationSPtoStarBranchNaked));
}