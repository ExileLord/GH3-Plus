#include "GH3Plus.h"
#include "GH3Keys.h"
#include "GH3GlobalAddresses.h"
#include "forceStrumNotes.h"

static LPVOID setHopoFlagDetour = (LPVOID)0x0041D22A;
static LPVOID setHopoFlagDetour2 = (LPVOID)0x0041D41B;

__declspec(naked) void forceTapFlagNaked()
{
    static const uint32_t returnAddress = 0x0041D233;
    __asm
    {
        mov [esp+20h],  0  //Tapping flag
        mov eax, KEY_PLAYER
        jmp returnAddress;
    }
}

__declspec(naked) void forceTapFlagNaked2()
{
    static const uint32_t returnAddress = 0x0041D423;
    __asm
    {
        mov edx, 0
        mov ecx, [esp+14h]
        jmp returnAddress
    }
}

void ApplyHack()
{
    gh3p::WriteJmp(setHopoFlagDetour2, &forceTapFlagNaked2);
   // gh3p::ScopedUnprotect unprotect((LPVOID)0x0041D290, 8);
   // *(char*)0x0041D294 = 0;
}