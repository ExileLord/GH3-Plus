#include "GH3Plus.h"
#include "GH3Keys.h"
#include "GH3GlobalAddresses.h"
#include "doubleNotes.h"

__declspec(naked) void tinyJump()
{
    static const uint32_t returnAddress = 0x0041D298;
    __asm
    {
        mov ecx, ebp;
        jmp returnAddress;
    }
}

void ApplyHack()
{
    gh3p::WriteJmp((LPVOID)0x0041D288, &tinyJump);
}