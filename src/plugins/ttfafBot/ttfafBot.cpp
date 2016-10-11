#include "GH3Plus.h"
#include "GH3Keys.h"
#include "GH3GlobalAddresses.h"
#include "tiltFix.h"
#include <WinBase.h>

//0042E194
static const LPDWORD starPowerCheckDetour = (LPDWORD)0x0042E194;


static bool g_isTilting[gh3p::GH3_MAX_PLAYERS] = { false };
static uint64_t g_tiltStart[gh3p::GH3_MAX_PLAYERS] = { 0 };
static const uint64_t g_timeThreshold = 300;
static float g_tiltAngleThreshold = -1.5;


//[esp+24] < tilt

bool __stdcall tiltFix(float tilt);

__declspec(naked) void tiltFixNaked()
{
    static const uint32_t returnAddressActivate = 0x0042E1B7;
    static const uint32_t returnAddressDontActivate = 0x0042E1DE;

    _asm
    {
        mov	eax, [esp + 0x24];
        pushad;

        push eax;
        call tiltFix;

        cmp eax, 0;
        jz DONT_ACTIVATE;

    ACTIVATE:
        popad;
        jmp returnAddressActivate;

    DONT_ACTIVATE:
        popad;
        jmp returnAddressDontActivate;
    };
}

bool __stdcall tiltFix(float tilt)
{
    uint32_t pIdx = 0; //temporary
    uint64_t currentTime = GetTickCount64();


    if (tilt > g_tiltAngleThreshold)
    {
        g_isTilting[pIdx] = false;
        g_tiltStart[pIdx] = currentTime;
        return false;
    }

    if (!g_isTilting[pIdx])
    {
        g_isTilting[pIdx] = true;
        g_tiltStart[pIdx] = currentTime;
    }
    else
    {
        if ((currentTime - g_tiltStart[pIdx]) > g_timeThreshold)
            return true;
    }

    return false;
}


void ApplyHack()
{
    gh3p::WriteJmp(starPowerCheckDetour, tiltFixNaked);
}