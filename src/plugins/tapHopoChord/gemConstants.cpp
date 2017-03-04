#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"

float g_gemStartPosX[6] = { 0 };
float g_gemStartPosY[6] = { 0 };
float g_gemEndPosX[6] = { 0 };
float g_gemEndPosY[6] = { 0 };

float g_gemLeftStartPosX[6] = { 0 };
float g_gemLeftStartPosY[6] = { 0 };
float g_gemLeftEndPosX[6] = { 0 };
float g_gemLeftEndPosY[6] = { 0 };

float g_gemLeftAngle[6] = { 0 };
float g_gemAngle[6] = { 0 };

uint32_t g_gemMatBattle[6] = { 0 };
uint32_t g_gemMatHammerBattle[6] = { 0 };

uint32_t g_gemMatStar[6] = { 0 };
uint32_t g_gemMatHammerStar[6] = { 0 };

uint32_t g_gemMatNormal[6] = { 0 };
uint32_t g_gemMatHammer[6] = { 0 };

uint32_t g_gemMatSp[6] = { 0 };
uint32_t g_gemMatHammerSp[6] = { 0 };

uint32_t g_gemMatWhammy[6] = { 0 };
uint32_t g_gemMatSpWhammy[6] = { 0 };

uint32_t g_gemMatTap[6] = { 0 };
uint32_t g_gemMatTapStar[6] = { 0 };
uint32_t g_gemMatTapSp[6] = { 0 };

void __declspec(naked) setupAnimNotes()
{
	__asm pushad;

	g_gemMatNormal[0] = GreenAnimTextureKey;
	g_gemMatNormal[1] = RedAnimTextureKey;
	g_gemMatNormal[2] = YellowAnimTextureKey;
	g_gemMatNormal[3] = BlueAnimTextureKey;
	g_gemMatNormal[4] = OrangeAnimTextureKey;

	g_gemMatHammer[0] = GreenHammerAnimTextureKey;
	g_gemMatHammer[1] = RedHammerAnimTextureKey;
	g_gemMatHammer[2] = YellowHammerAnimTextureKey;
	g_gemMatHammer[3] = BlueHammerAnimTextureKey;
	g_gemMatHammer[4] = OrangeHammerAnimTextureKey;

	g_gemMatSp[0] = StarpowerAnimTextureKey;
	g_gemMatSp[1] = StarpowerAnimTextureKey;
	g_gemMatSp[2] = StarpowerAnimTextureKey;
	g_gemMatSp[3] = StarpowerAnimTextureKey;
	g_gemMatSp[4] = StarpowerAnimTextureKey;

	g_gemMatHammerSp[0] = StarpowerHammerAnimTextureKey;
	g_gemMatHammerSp[1] = StarpowerHammerAnimTextureKey;
	g_gemMatHammerSp[2] = StarpowerHammerAnimTextureKey;
	g_gemMatHammerSp[3] = StarpowerHammerAnimTextureKey;
	g_gemMatHammerSp[4] = StarpowerHammerAnimTextureKey;

	g_gemMatTap[0] = GreenTapTextureKey;
	g_gemMatTap[1] = RedTapTextureKey;
	g_gemMatTap[2] = YellowTapTextureKey;
	g_gemMatTap[3] = BlueTapTextureKey;
	g_gemMatTap[4] = OrangeTapTextureKey;

	g_gemMatTapStar[0] = GreenTapStarTextureKey;
	g_gemMatTapStar[1] = RedTapStarTextureKey;
	g_gemMatTapStar[2] = YellowTapStarTextureKey;
	g_gemMatTapStar[3] = BlueTapStarTextureKey;
	g_gemMatTapStar[4] = OrangeTapStarTextureKey;

	g_gemMatTapSp[0] = StarpowerTapTextureKey;
	g_gemMatTapSp[1] = StarpowerTapTextureKey;
	g_gemMatTapSp[2] = StarpowerTapTextureKey;
	g_gemMatTapSp[3] = StarpowerTapTextureKey;
	g_gemMatTapSp[4] = StarpowerTapTextureKey;

	__asm popad;
	__asm retn;
}

void __declspec(naked) setupOpenNote()
{
	__asm pushad;

	//Copy most of the yellow yellow note properties
	g_gemStartPosX[5] = g_gemStartPosX[2];
	g_gemStartPosY[5] = g_gemStartPosY[2];
	g_gemEndPosX[5] = g_gemEndPosX[2];
	g_gemEndPosY[5] = g_gemEndPosY[2];
	g_gemLeftStartPosX[5] = g_gemLeftStartPosX[2];
	g_gemLeftStartPosY[5] = g_gemLeftStartPosY[2];
	g_gemLeftEndPosX[5] = g_gemLeftEndPosX[2];
	g_gemLeftEndPosY[5] = g_gemLeftEndPosY[2];
	g_gemLeftAngle[5] = g_gemLeftAngle[2];
	g_gemAngle[5] = g_gemAngle[2];

	g_gemMatBattle[5] = OpenStarTextureKey;
	g_gemMatHammerBattle[5] = OpenStarHammerTextureKey;
	g_gemMatStar[5] = OpenStarTextureKey;
	g_gemMatHammerStar[5] = OpenStarHammerTextureKey;
	g_gemMatNormal[5] = OpenTextureKey;
	g_gemMatHammer[5] = OpenHammerTextureKey;
	g_gemMatSp[5] = OpenStarpowerTextureKey;
	g_gemMatHammerSp[5] = OpenStarpowerHammerTextureKey;
	g_gemMatWhammy[5] = g_gemMatWhammy[2];
	g_gemMatSpWhammy[5] = g_gemMatSpWhammy[2];

	g_gemMatTap[5] = OpenHammerTextureKey;
	g_gemMatTapStar[5] = OpenStarHammerTextureKey;
	g_gemMatTapSp[5] = OpenStarpowerHammerTextureKey;

	__asm popad;
	__asm retn;
}

static void * const gemConstantFixingDetour = (void *)0x0041BB91;
void __declspec(naked) gemConstantFixingNaked()
{
	static void * const loopAddress = (void *)0x0041B954;
	static void * const returnAddress = (void *)0x0041BC62;
	__asm
	{
		fstp	[g_gemLeftStartPosX + esi];
		mov     ecx, [esp + 48h];
		fld     dword ptr[eax + 8];
		mov     eax, [esp + 18h];
		fstp	[g_gemLeftStartPosY + esi];
		movss   xmm0, dword ptr[esp + 1Ch];
		fld     dword ptr[eax + 4];
		fstp	[g_gemLeftEndPosX + esi];
		fld     dword ptr[eax + 8];
		mov     eax, [esp + 44h];
		fstp	[g_gemLeftEndPosY + esi];
		fld     dword ptr[ebp + 4];
		fstp	[g_gemStartPosX + esi];
		fld     dword ptr[ebp + 8];
		fstp	[g_gemStartPosY + esi];
		fld     dword ptr[ebx + 4];
		fstp	[g_gemEndPosX + esi];
		fld     dword ptr[ebx + 8];
		mov		[g_gemMatHammerSp + esi], edx;
		mov     edx, [esp + 4Ch];
		fstp	[g_gemEndPosY + esi];
		mov		[g_gemMatSp + esi], eax;
		mov     eax, [esp + 50h];
		mov		[g_gemMatHammerBattle + esi], ecx;
		mov     ecx, [esp + 54h];
		mov		[g_gemMatBattle + esi], edx;
		mov     edx, [esp + 58h];
		mov		[g_gemMatHammerStar + esi], eax;
		mov     eax, [esp + 5Ch];
		mov		[g_gemMatStar + esi], ecx;
		mov     ecx, [esp + 60h];
		mov		[g_gemMatHammer + esi], edx;
		mov     edx, [esp + 64h];
		movss	[g_gemLeftAngle + esi], xmm0;
		movss   xmm0, dword ptr[esp + 20h];
		mov		[g_gemMatNormal + esi], eax;
		movss	[g_gemAngle + esi], xmm0;
		mov		[g_gemMatSpWhammy + esi], ecx;
		mov		[g_gemMatWhammy + esi], edx;

		add     esi, 4;
		cmp     esi, 20;
		jl      LOOP_ADDRESS;
		call    setupAnimNotes;
		call    setupOpenNote;
		jmp     returnAddress;
	LOOP_ADDRESS:
		jmp		loopAddress;
	}
}








bool TryApplyGemConstantsPatches()
{
	return g_patcher.WriteJmp(gemConstantFixingDetour, gemConstantFixingNaked);
}