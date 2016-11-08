#pragma once

#include "core\Patcher.h"
#include "gh3\QbArray.h"


///Determines which tapping gem sprite to use under starpower since we don't have a starpower texture
//Set this to 0 to use the green note texture
//            1 to use the red note texture
//            2 to use the yellow note texture
//            3 to use the blue note texture
//            4 to use the orange note texture
#define SP_TAP_NOTE_GEM_INDEX 3

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

enum HopoFlag : uint32_t
{
	STRUMMED = 0,
	HOPO = 1,
	TAPPING = 2
};

struct Note
{
	uint32_t vTable;
	uint32_t fret[5];
	HopoFlag hopoFlag;
};


enum FretMask : uint32_t
{
	ORANGE = 0x00001,
	BLUE = 0x00010,
	YELLOW = 0x00100,
	RED = 0x01000,
	GREEN = 0x10000,
	OPEN = 0x22222,
};

enum QbFretMask : uint32_t
{
	QbGreen = 0x1,
	QbRed = 0x2,
	QbYellow = 0x4,
	QbBlue = 0x8,
	QbOrange = 0x10,
	QbHopoflip = 0x20,
	QbTapping = 0x40,
	QbOpen = 0x80
};

extern GH3P::Patcher g_patcher;



extern float g_gemStartPosX[6];
extern float g_gemStartPosY[6];
extern float g_gemEndPosX[6];
extern float g_gemEndPosY[6];

extern float g_gemLeftStartPosX[6];
extern float g_gemLeftStartPosY[6];
extern float g_gemLeftEndPosX[6];
extern float g_gemLeftEndPosY[6];

extern float g_gemLeftAngle[6];
extern float g_gemAngle[6];

extern uint32_t g_gemMatBattle[6];
extern uint32_t g_gemMatHammerBattle[6];

extern uint32_t g_gemMatStar[6];
extern uint32_t g_gemMatHammerStar[6];

extern uint32_t g_gemMatNormal[6];
extern uint32_t g_gemMatHammer[6];

extern uint32_t g_gemMatSp[6];
extern uint32_t g_gemMatHammerSp[6];

extern uint32_t g_gemMatWhammy[6];
extern uint32_t g_gemMatSpWhammy[6];


void ApplyHack();

bool TryApplyNoteLogicPatches();
bool TryApplyNoteLoadingPatches();
bool TryApplyGemLoadingPatches();
bool TryApplyGemMutationPatches();
bool TryApplyGlobalArrayPatches();

FretMask getHighFret(FretMask fretMask);