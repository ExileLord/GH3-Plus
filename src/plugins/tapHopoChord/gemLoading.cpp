#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"


///////////////////////////////
// Primary gem loading logic //
///////////////////////////////

static void * const setStarGemAppearanceDetour = (void *)0x004328C2;
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
			cmp dword ptr[eax], KEY_P2_BATTLE;
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

static void * const setNormalGemAppearanceDetour = (void *)0x0043293A;
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

static void * const setSpGemAppearanceDetour = (void *)0x0043289E;
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

		//Tap notes
		mov		eax, SP_TAP_NOTE_GEM_INDEX;
		mov     edi, [ADDR_gemMatBattle2 + eax * 4]; // g_gemMatHammerSp2[eax * 4]
		jmp		returnAddress;

	NORMAL_NOTE:
		mov     edi, [eax * 4 + 0x00A1787C]; // g_gemMatHammerSp2[eax * 4]
		jmp returnAddress;
	}
}

bool TryApplyGemLoadingPatches()
{
	return (g_patcher.WriteJmp(setStarGemAppearanceDetour, &makeStarTapGemsNaked) &&
			g_patcher.WriteJmp(setNormalGemAppearanceDetour, &makeTapGemsNaked) &&
			g_patcher.WriteJmp(setSpGemAppearanceDetour, &makeSpTapGemsNaked));
}