#include "tiltFix.h"
#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include <stdint.h>
#include <Windows.h>

const uint32_t GH3_MAX_PLAYERS = 2;

//Detours
static void * const starPowerCheckDetour = (void *)0x0042E194;

// Static variables used by hack
static bool g_isTilting[GH3_MAX_PLAYERS] = { false };
static uint64_t g_tiltStart[GH3_MAX_PLAYERS] = { 0 };

static uint64_t g_timeThreshold = 266; //266 the equivalent of 16 frames at 60fps which is the default tilt threshold.
static float g_tiltAngleThreshold = -1.0f; //-1.0 is the highest the controller can be tilted with 1.0 being the lowest. Set this to a value less than -1.0 to never activate by tilting.

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);



bool __stdcall tiltFix(float tilt);

__declspec(naked) void tiltFixNaked()
{
    static const uint32_t returnAddressActivate = 0x0042E1B7;
    static const uint32_t returnAddressDontActivate = 0x0042E1DE;

    _asm
    {
        mov	eax, [esp + 0x24]; // tilt
        pushad;

        push eax;
        call tiltFix;

        cmp eax, 0;
        jz DONT_ACTIVATE;

    //ACTIVATE:
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
	uint64_t currentTime;

	memset(&currentTime, 0, sizeof(currentTime)); //Fix a bug with the compiler and linker only setting the lower 32-bits of the time.
	currentTime = GetTickCount64();

    if (tilt > g_tiltAngleThreshold)
    {
        g_isTilting[pIdx] = false;
        g_tiltStart[pIdx] = currentTime;
    }
	else if (!g_isTilting[pIdx])
    {
        g_isTilting[pIdx] = true;
        g_tiltStart[pIdx] = currentTime;
    }
    else if ((currentTime - g_tiltStart[pIdx]) > g_timeThreshold)
    {
        return true;
    }

    return false;
}


void ApplyHack()
{
    g_patcher.WriteJmp(starPowerCheckDetour, tiltFixNaked);
}