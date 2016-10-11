#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "core\Patcher.h"
#include "doubleNotes.h"
#include <stdint.h>

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

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
    g_patcher.WriteJmp((void *)0x0041D288, &tinyJump);
}