#include "keyHijacker.h"
#include "resource.h"
#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
//#include <WinBase.h>
#include <Windows.h>
#include <mmsystem.h>
#include <string>
#include <cstdio>
#pragma comment(lib,"Winmm.lib")

static const LPVOID changeDetour = (LPVOID)0x00538EF4;


static uint32_t g_lastKey = 0x00000000;
static uint32_t g_lastFlag = 0x00000000;
static uint32_t g_lastValue = 0x00000000;

static uint32_t g_lastFloatKey = 0x00000000;
static uint32_t g_probableSongSpeed = 0x00000000;
static uint32_t g_probableSongPitch = 0x00000000;

static float g_hackedSpeed = 1.0f;
static float g_hackedPitch = 1.0f;
static int32_t g_currentInt = 100;
static int32_t g_multiplier = 0;
static bool g_keyHeld = false;

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

BOOL PlayResource(LPCWSTR lpName)
{
    BOOL bRtn;
    LPWSTR lpRes;
    HRSRC hResInfo;
    HGLOBAL hRes;

    // Find the WAVE resource. 

    hResInfo = FindResource(NULL, lpName, L"WAVE");
    if (hResInfo == NULL)
        return FALSE;

    // Load the WAVE resource. 

    hRes = LoadResource(NULL, hResInfo);
    if (hRes == NULL)
        return FALSE;

    // Lock the WAVE resource and play it. 

    lpRes = (LPWSTR)LockResource(hRes);
    if (lpRes != NULL) {
        bRtn = sndPlaySound(lpRes, SND_MEMORY | SND_SYNC | SND_NODEFAULT);
        UnlockResource(hRes);
    }
    else
        bRtn = 0;

    // Free the WAVE resource and return success or failure. 

    FreeResource(hRes);
    return bRtn;
}

__declspec(naked) void changeOverrideNaked()
{
    static const uint32_t returnAddress = 0x538EFA; // TODO
    static const uint32_t speedFactorKey = 0x16D91BC1;
    static const uint32_t structureNameKey = 0xFD2C9E38;
    static const uint32_t pitchStructKey = 0x3370A847;
    static const uint32_t pitchKey = 0xD8604126;

    __asm
    {
        // Copied start of "change"
        mov     ebp, [esp + 36]; // aList
        push    esi;
        push    edi;
        
        // Check for current_speedfactor
        mov     ecx, dword ptr[ebp + 4]; // Using edi and ecx because they are overwritten by
        mov     edi, dword ptr[ecx + 4]; // existing code immediately after jumping back
        cmp     edi, speedFactorKey;     
        je      OVERWRITESPEED;

        // Check for structurename
        cmp     edi, structureNameKey;
        je      CHECKPITCHSTRUCT;
        jmp     returnAddress;

    CHECKPITCHSTRUCT:
        mov     edi, dword ptr[ecx + 8];
        cmp     edi, pitchStructKey;
        je      CHECKPITCH;
        jmp     returnAddress;

    CHECKPITCH:
        mov     edi, dword ptr[ecx + 12]; // Next node of the linked list
        mov     ecx, dword ptr[edi + 4];
        cmp     ecx, pitchKey;
        je      OVERWRITEPITCH;
        jmp     returnAddress;

    OVERWRITESPEED:
        mov     edi, g_hackedSpeed;
        mov     dword ptr[ecx + 8], edi;
        jmp     returnAddress;

    OVERWRITEPITCH:
        mov     ecx, g_hackedPitch;
        mov     dword ptr[edi + 8], ecx;
        jmp     returnAddress;
    }
}

int32_t getKeypadNumber();

void applyNewSpeed( int32_t newSpeed )
{
    if (newSpeed <= 0)
        return;
    g_hackedSpeed = (float)(newSpeed / 100.0f);
    g_hackedPitch = 1.0f / g_hackedSpeed;
}

void resetKeys()
{
    g_currentInt = 100;
    g_multiplier = 0;
}

void checkKeys()
{
    int number = getKeypadNumber();

    if (!g_keyHeld && number >= 0)
    {
        g_keyHeld = true;
        g_multiplier += (g_currentInt * number);
        g_currentInt /= 10;

        if (g_currentInt == 0)
        {
            applyNewSpeed(g_multiplier);
            resetKeys();
            
            //PlayResource(MAKEINTRESOURCE(IDR_WAVE1));
            //PlaySound(MAKEINTRESOURCE(IDR_WAVE1), NULL, SND_RESOURCE | SND_ASYNC);
            PlaySound(TEXT("quack.wav"), NULL, SND_FILENAME | SND_ASYNC);
        }

    }

    if (g_keyHeld && number == -1)
    {
        g_keyHeld = false;
    }
}

int32_t getKeypadNumber()
{
    static uint8_t keys[256];
    memset(keys, 0, sizeof(keys));
    GetKeyboardState(keys);

    for (int i = 0; i < 10; ++i)
    {
        if ( (keys[VK_NUMPAD0 + i] & 0x80) || (keys['0' + i]) & 0x80)
            return i;
    }

    return -1;
}

static const LPVOID gameFrameDetour = (LPVOID)0x005B0C50;
_declspec(naked) void checkKeysNaked()
{
    static const uint32_t returnAddress = 0x005B0C59;

    __asm
    {
        pushad;
        call checkKeys;
        popad;

        //Displaced code
        mov     eax, [ecx + 8];
        and     eax, 0Fh;
        add     eax, 0FFFFFFFFh;

        jmp returnAddress;
    }
}



void ApplyHack()
{
    //gh3p::WriteJmp(GetDWord, &storeLastKeyNaked);
    //gh3p::WriteJmp(GetFloatDetour, &storeFloatKeyNaked);
    //gh3p::WriteJmp(HijackGetFloatDetour, &hijackGetFloatNaked);
    g_patcher.WriteJmp(changeDetour, &changeOverrideNaked);
    g_patcher.WriteJmp(gameFrameDetour, &checkKeysNaked);
}