#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"
#include "core\QBKey.h"

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
