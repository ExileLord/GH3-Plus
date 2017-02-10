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

static const LPVOID GetDWord = (LPVOID)0x004786A0;
static const LPVOID GetFloatDetour = (LPVOID)0x00478A48;
static const LPVOID HijackGetFloatDetour = (LPVOID)0x00478A5A;


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


static const LPVOID SetCase2Detour = (LPVOID)0x00539245;
__declspec(naked) void hijackCase2Naked()
{
    static const uint32_t returnAddress = 0x0053924E;

    __asm
    {
        mov     byte ptr[ebx + 2], 2;
        mov     eax, g_probableSongSpeed;
        sub     eax, 0xC;
        cmp     eax, ebx;
        jz      HIJACKSPEED

//         mov     eax, g_probableSongPitch;
//         sub     eax, 0xC;
//         cmp     eax, ebx;
//         jz      HIJACKPITCH

        movss   dword ptr[ebx + 0xC], xmm0;

        mov     eax, [ebx + 0xC];
        cmp     eax, 0x3F2AAAAC; //0.6666
        jz      STORESPEED

//         cmp     eax, 0x3FC00000; //1.5
//         jz      STOREPITCH

        jmp     returnAddress;

    STORESPEED:
        mov     eax, g_probableSongSpeed;
        cmp     eax, 0;
        jnz     DONTSTORE;

        lea     eax, [ebx + 0xC];
        cmp     eax, 0x0B001111;
        jle     DONTSTORE;
        cmp     eax, 0x0F001111;
        jge     DONTSTORE;

        mov     g_probableSongSpeed, eax;
        jmp     returnAddress;
    
//     STOREPITCH:
//         mov     eax, g_probableSongPitch;
//         cmp     eax, 0;
//         jnz     DONTSTORE;
//         mov     eax, g_probableSongSpeed; //Don't store if game speed hasn't been found
//         cmp     eax, 0;
//         jz      DONTSTORE;
// 
//         lea     eax, [ebx + 0xC];
//         cmp     eax, 0x0A000000;
//         jle     DONTSTORE;
//         cmp     eax, 0x0C000000;
//         jge     DONTSTORE;
// 
//         mov     g_probableSongPitch, eax;
//         jmp     returnAddress;

    DONTSTORE:
        jmp     returnAddress;

    HIJACKSPEED:
        mov     eax, g_hackedSpeed;
        mov     [ebx + 0xC], eax;
        jmp     returnAddress;

//     HIJACKPITCH:
//         mov     eax, g_hackedPitch;
//         mov[ebx + 0xC], eax;
//         jmp     returnAddress;
    }
}

//00539004 <- where pitch gets set yo







int32_t getKeypadNumber();

void applyNewSpeed( int32_t newSpeed )
{
    if (newSpeed <= 0)
        return;
    g_hackedSpeed = (float)(newSpeed / 100.0f);
    g_hackedPitch = 1.0f / g_hackedSpeed;
    
    if (g_probableSongSpeed != 0x00000000)
        *((float *)g_probableSongSpeed) = g_hackedSpeed;

    if (g_probableSongPitch != 0x00000000)
        *((float *)g_probableSongPitch) = g_hackedPitch;
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
	if (!g_patcher.WriteJmp(SetCase2Detour, &hijackCase2Naked) ||
		g_patcher.WriteJmp(gameFrameDetour, &checkKeysNaked))
	{
		g_patcher.RemoveAllChanges();
	}

    
}