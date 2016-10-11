// GH3Hack.cpp : Defines the exported functions for the DLL application.
//

#include "mirrorMode.h"
#include "core\Patcher.h"
#include <stdint.h>


static void * const CreateNote = (void *)0x0041AB60;

static void * const createNoteDetour = (void *)0x0041D41B;
static void * const getFretsForHopoCheckDetour = (void *)0x0041D1D4;

uint32_t __stdcall getFretsFromUnkReverse(int *noteThis);

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

enum class EFretMask : uint32_t
{
    ORANGE = 0x00001,
    BLUE =   0x00010,
    YELLOW = 0x00100,
    RED =    0x01000,
    GREEN =  0x10000
};

__declspec (naked) void createNoteReversedNaked()
{
    /*
    -00000038 yellow1         dd ?
    -00000034 blue1           dd ?
    -00000030 red1            dd ?
    -0000002C orange1         dd ?
    -00000028 hopoRaw         dd ?
    -00000024 green1          dd ?
    */
    static const uint32_t returnAddress = 0x0041D449;

    __asm
    {
        mov     edx, [esp + 20h]; //[esp + 48h + hopoRaw];
        mov     ecx, [esp + 18h]; //RED //[esp + 48h + blue1];
        mov     esi, [esp + 30h]; //[esp + 48h + noteIndex];
        push    eax; //a10
        mov     eax, [esp + 28h]; //GREEN //[esp + 4Ch + orange1];
        push    edx; //a9
        mov     edx, [esp + 18h]; //[esp + 50h + yellow1];
        push    eax; //a8
        mov     eax, [esp + 20h];//BLUE //[esp + 54h + red1];
        push    ecx; //a7
        mov     ecx, [esp + 2Ch];//ORANGE //[esp + 58h + green1]; //
        push    edx; //a6
        mov     edx, [esp + 40h]//[esp + 5Ch + a3]; //
        push    eax; //a5
        mov     eax, [esp + 4Ch ]//[esp + 60h + notesObj]; //noteCollectionStruct
        push    ecx; //a4
        push    edx; //a3
        mov     ecx, esi; //noteIndex
        jmp     returnAddress;
    }
}
__declspec(naked) void getFretsFromUnkReverseNaked()
{
    static const uint32_t returnAddress = 0x0041D1D9;
    __asm
    {
        pushad;
        push ecx;
        call getFretsFromUnkReverse;
        popad;
        jmp returnAddress;
    }
    
}

//This is more or less an exact translation of GetFretsFromUnk except it's not __thiscall and the frets are reversed
uint32_t __stdcall getFretsFromUnkReverse(int *noteThis)
{
    int flag; // esi@1
    int *note; // ecx@1
    uint32_t result; // eax@1
    int green; // edx@1
    int red; // edx@5
    int yellow; // edx@9
    int blue; // edx@13

    flag = noteThis[1];
    note = (int *)noteThis[2];
    result = 0;                                   // 
    // 
    green = (int)note;
    if (flag != 1)
        green = note[1];
    if (green > 0)
        result = (uint32_t)EFretMask::ORANGE;                             // 
    // 
    red = (int)note;
    if (flag != 1)
        red = note[2];
    if (red > 0)
        result |= (uint32_t)EFretMask::BLUE;                              // 
    // 
    yellow = (int)note;
    if (flag != 1)
        yellow = note[3];
    if (yellow > 0)
        result |= (uint32_t)EFretMask::YELLOW;                           // 
    // 
    blue = (int)note;
    if (flag != 1)
        blue = note[4];
    if (blue > 0)
        result |= (uint32_t)EFretMask::RED;                             // 
    // 
    if (flag != 1)
        note = (int *)note[5];
    if ((signed int)note > 0)
        result |= (uint32_t)EFretMask::GREEN;                           // 
    // 
    return result;
}

void ApplyHack()
{
    g_patcher.WriteJmp(createNoteDetour, &createNoteReversedNaked);
    g_patcher.WriteJmp(getFretsForHopoCheckDetour, &getFretsFromUnkReverseNaked);
}

