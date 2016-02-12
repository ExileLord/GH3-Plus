#include "GH3Plus.h"
#include "GH3Keys.h"
#include "GH3GlobalAddresses.h"
#include "tapHopoChord.h"

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

//Types

enum class EHopoFlag : uint32_t
{
    STRUMMED = 0,
    HOPO = 1,
    TAPPING = 2
};

struct Note
{
    uint32_t vTable;
    uint32_t fret[5];
    EHopoFlag hopoFlag;
};


//Detour addresses
static const LPDWORD cave1DetourAddress = (LPDWORD)0x0041D22E;
static const LPDWORD guitarInputLogicDetour1 = (LPDWORD)0x00431A79;
static const LPDWORD guitarInputLogicDetour2 = (LPDWORD)0x00431B35;
static const LPDWORD guitarInputLogicDetour2b = (LPDWORD)0x00431906;
static const LPDWORD chordHitCheckDetour = (LPDWORD)0x00431EA6;
static const LPDWORD noteHitEndDetour = (LPDWORD)0x004320AD;
static const LPDWORD noteHitEndDetour2 = (LPDWORD)0x0043214C;
static const LPDWORD setStarGemAppearanceDetour = (LPDWORD)0x004328C2;
static const LPDWORD setNormalGemAppearanceDetour = (LPDWORD)0x0043293A;
static const LPDWORD setSpGemAppearanceDetour = (LPDWORD)0x0043289E;

//GH3 functions
static const LPDWORD ProbablyGetDWord = (LPDWORD)0x0040A9B0;

//GH3 globals
static uint32_t *g_hitFrets = (uint32_t *)ADDR_hitFrets;
static uint32_t **g_crazyPointerChain = (uint32_t **)ADDR_CrazyPointerChain;
static uint32_t *g_currentNote = (uint32_t *)ADDR_currentNote;
static uint32_t *g_pressedFrets = (uint32_t *)ADDR_pressedFrets;
static uint32_t *g_hitNoNotes_bool = (uint32_t *)ADDR_hitNoNotes_bool;

//Hack globals
static uint32_t g_canTap[gh3p::GH3_MAX_PLAYERS] = { 0 };
static uint32_t g_canTapLow[gh3p::GH3_MAX_PLAYERS] = { 0 };
static uint32_t g_lastHit[gh3p::GH3_MAX_PLAYERS] = { 0 };

//Pre-declarations
uint32_t *LoadCurrentNoteStruct(uint32_t pIdx);
uint32_t getHighFret(uint32_t fretMask);
uint32_t getAnchoredMask(uint32_t fretMask);
bool hitHopoChord(uint32_t fretsHeld, uint32_t noteMask);
bool hitHopoChordHigh(uint32_t fretsHeld, uint32_t noteMask);
bool hitHopoChordLow(uint32_t fretsHeld, uint32_t noteMask);
uint32_t __stdcall loadLastHit(uint32_t pIdx);
void __stdcall setCanTapState(uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx);
int __stdcall chordHitCheck(EHopoFlag hopoFlag, uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx);
void __stdcall noteHitEnd(uint32_t pIdx);


////////////////
// ~~~~~~~~~~ //
////////////////


// Injected into the code loading an individual note
__declspec( naked ) void loadTappingFlagNaked()
{
    static const int frameOffset = 0x48 + 0x4*8; //push an addition 4 for each offset register

    //Needed stack variables of GuitarInputFrame
    static const int arg_C = 0x10 + frameOffset;
    static const int obj = 0x04 + frameOffset;
    static const int hopoRaw = -0x28 + frameOffset;

    static const unsigned int returnAddress = 0x0041D239;

    //Too fucking lazy to convert this to C
    //It's just loading the tapping flag and setting hopo flag to 2 if it has one
    _asm
    {
       pushad

       mov      eax, [esp + 0x78] //[esp + arg_C]
       mov      edx, [eax + 4]
       mov      eax, [eax + 8]
       mov      edi, eax
       mov      ecx, eax
       cmp      edx, 1
       jz       branch_a
       mov      edi, [eax + esi * 4 + 4]
       mov      ecx, [eax + esi * 4 + 8]
                
branch_a:       
       sar      ecx, 6
       and      ecx, 1
       cmp      ecx, 1
       jnz      branch_b
       mov      edi, 2
       mov      [esp + 0x40], edi //[esp + hopoRaw]

branch_b:

       popad;

       mov     eax, KEY_PLAYER ; //Existing code we pushed out of the way to jump to this cave
       push    eax
       call    ProbablyGetDWord

       jmp     returnAddress   ; //Return to normal execution

    };
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void loadLastHitNaked()
{
    static const uint32_t returnAddress = 0x00431A80;

    _asm
    {
        PUSH_ALL_BUT_EDX

        push ecx; //pIdx
        call loadLastHit;
        mov edx, eax;
        
        POP_ALL_BUT_EDX

        jmp returnAddress;
    };
}

uint32_t __stdcall loadLastHit(uint32_t pIdx)
{
    EHopoFlag hopoFlag;

    uint32_t *noteStruct = LoadCurrentNoteStruct(pIdx);
    uint32_t fretCount = 0;
    bool isChord = false;

    //Get the hopo flag out of the note structure
    if (noteStruct[1] == 1)
    {
        //1D Case (this probably doesn't ever get hit because it seems like insanity)
        EHopoFlag *flagPtr = reinterpret_cast<EHopoFlag*>(noteStruct);
        hopoFlag = flagPtr[2];
    }
    else
    {
        //2D Case
        Note &note = *reinterpret_cast<Note *>(noteStruct[2]);
        hopoFlag = note.hopoFlag;
        for (int i = 0; i < 5; ++i)
        {
            if (note.fret[i] == 1)
                ++fretCount;
        }
        if (fretCount > 1)
            isChord = true;
    }

    //Accounting for chords here fixes an edge case that would be painful to fix in later code
    if (hopoFlag == EHopoFlag::HOPO && g_canTapLow[pIdx] && g_hitFrets[pIdx] != 0x00000 && isChord) //tapping note
    {
        return 0x11111111;
    }

    if (hopoFlag == EHopoFlag::TAPPING && (g_canTap[pIdx] || (g_canTapLow[pIdx] && isChord))) //tapping note
    {
        return 0x11111111;
    }
    else
    {
        return g_hitFrets[pIdx];
    }

}



uint32_t *LoadCurrentNoteStruct(uint32_t pIdx)
{

    uint32_t *playerChain = g_crazyPointerChain[pIdx];
    if (playerChain[1] == 1)
    {
        uint32_t **playerAltChain = reinterpret_cast<uint32_t **>(playerChain);
        return playerAltChain[2];
    }
    else
    {
        uint32_t ***playerAltChain = reinterpret_cast<uint32_t ***>(playerChain);
        return playerAltChain[2][g_currentNote[pIdx]];
    }
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void setCanTapStateNaked()
{
    static const uint32_t returnAddress = 0x00431B43;

    _asm
    {
        pushad;
        push ecx;                         //arg 3 pIdx
        push ebx;                         //arg 2 currFrets
        mov eax, [ADDR_pressedFrets + ecx * 4]; 
        push eax;                         //arg 1 prevFrets
        call setCanTapState;
        popad;

        mov eax, [ADDR_CrazyPointerChain + ecx * 4];
        mov [ADDR_pressedFrets + ecx * 4], ebx;
        jmp returnAddress;
    };
}

void __stdcall setCanTapState(uint32_t currFrets, uint32_t prevFrets, uint32_t pIdx)
{
    if (getHighFret(currFrets) != getHighFret(prevFrets))
        g_canTap[pIdx] = true;

    if (currFrets != prevFrets)
        g_canTapLow[pIdx] = true;
}

uint32_t getHighFret(uint32_t fretMask)
{
    if (fretMask & 0x00001)
        return  0x00001;

    if (fretMask & 0x00010)
        return  0x00010;

    if (fretMask & 0x00100)
        return  0x00100;

    if (fretMask & 0x01000)
        return  0x01000;

    if (fretMask & 0x10000)
        return  0x10000;

    return 0x00000;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void chordHitCheckNaked()
{

    static const uint32_t CHORD_CHECK_FAILED = 0x00431EE3;
    static const uint32_t CHORD_CHECK_PASSED = 0x00431EBA;
    static const uint32_t NOTE_HIT_HOPO = 0x00431F50;

    static const int frameOffset = 0x198;

    static const int isHopo2 = -0x164 + frameOffset; //0x34
    static const int pIdx = -0x170 + frameOffset; //0x28
    static const int pIdx9 = -0x188 + frameOffset; //0x10

    __asm
    {
        mov ecx, [esp + 0x10];
        push ecx                     // pIdx
        push ebx;                    // note mask
        push [ADDR_pressedFrets + ecx * 4] // pressed frets
        push[esp + 0x40];        // hopo flag
        call chordHitCheck;
        cmp eax, 1;
        je L_NOTE_HIT_HOPO;

    //L_CHORD_FIRST_CHECK:; //Normal chord. Not HOPO (or hopo failed)
        cmp		byte ptr[esp + 17h], 0;  //var_184 + 3. I don't know what this is
        jnz		L_CHORD_CHECK_PASSED
        mov		ecx, [esp + 0x10]; //pIdx9 
        cmp     [ADDR_hitNoNotes_bool + ecx * 4], edi; 
        jnz		L_CHORD_CHECK_FAILED
        jmp		CHORD_CHECK_PASSED

        //Because MASM doesn't support conditional direct jumps
    L_CHORD_CHECK_FAILED:
        jmp		CHORD_CHECK_FAILED;
    L_CHORD_CHECK_PASSED:
        jmp		CHORD_CHECK_PASSED;
    L_NOTE_HIT_HOPO:
        jmp		NOTE_HIT_HOPO;
    }
}

int __stdcall chordHitCheck(EHopoFlag hopoFlag, uint32_t fretsHeld, uint32_t noteMask, uint32_t pIdx)
{
    if (hopoFlag == EHopoFlag::STRUMMED)
        return 0;

    if (g_canTap[pIdx] && hitHopoChordHigh(fretsHeld, noteMask))
        return 1;

    if (g_canTapLow[pIdx] && hitHopoChordLow(fretsHeld, noteMask))
        return 1;

    return 0;
}

bool hitHopoChordHigh(uint32_t fretsHeld, uint32_t noteMask)
{
    if ((fretsHeld & noteMask) != noteMask)
        return false;

    if ((fretsHeld & getAnchoredMask(noteMask)) != fretsHeld)
        return false;

    return true;
}

bool hitHopoChordLow(uint32_t fretsHeld, uint32_t noteMask)
{
    return (fretsHeld == noteMask);
}


uint32_t getAnchoredMask(uint32_t fretMask)
{
    if (fretMask >= 0x10000)
        return fretMask;

    if (fretMask >= 0x01000)
        return fretMask | 0x10000;

    if (fretMask >= 0x00100)
        return fretMask | 0x11000;

    if (fretMask >= 0x00010)
        return fretMask | 0x11100;

    if (fretMask >= 0x00001)
        return fretMask | 0x11110;

    return 0x11111;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////



__declspec(naked) void noteHitEndNaked()
{
    const static uint32_t normalReturnAddress = 0x00432166;
    const static uint32_t debugReturnAddress = 0x00432153;

    __asm
    {
        pushad;
        mov eax, [esp + 30h]; //pIdx9 + 20h
        push eax;
        call noteHitEnd;
        popad;

        //original displaced code
        cmp dword ptr[esp + 2Ch], 0; //198h + input.field_0
        jz L_NOT_DEBUG;
        jmp debugReturnAddress;
    L_NOT_DEBUG:
        jmp normalReturnAddress;
    }
}

void __stdcall noteHitEnd(uint32_t pIdx)
{
    g_lastHit[pIdx] = g_pressedFrets[pIdx];
    g_canTap[pIdx] = 0;
    g_canTapLow[pIdx] = 0;
}



////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////

////////////////
// ~~~~~~~~~~ //
////////////////
 


__declspec(naked) void makeStarTapGemsNaked()
{
    const static uint32_t tappingReturn = 0x00432901;

    const static uint32_t bossReturn = 0x004328ED;
    const static uint32_t normalReturn = 0x004328D7;
    
    //edx = 1
    //edi = gemMaterial2
    //eax = gemIndex
    //ebp = hammerOn flag
    // This is easier to do in asm since it's so simple. The bulk of this is moving the original logic around
    __asm
    {
        cmp ebp, edx; //cmp hopo flag vs 1
        jle L_NOT_TAPPING; //if 0 or 1 then it's strummed or hopo

        //tapping note
        mov edi, [ADDR_gemMatHammerBattle2 + eax * 4];
        jmp tappingReturn;

    L_NOT_TAPPING:
        push eax
        mov eax, ADDR_gameMode
        cmp dword ptr [eax], KEY_P2_BATTLE;
        je L_BOSS_BATTLE;
        mov eax, ADDR_bossBattle
        cmp dword ptr[eax], 0;
        jnz L_BOSS_BATTLE;

        pop eax
        jmp normalReturn;

    L_BOSS_BATTLE:
        pop eax;
        jmp bossReturn;
    }
}

__declspec(naked) void makeTapGemsNaked()
{
    const static uint32_t returnAddress = 0x0043294E;
    __asm
    {
        cmp     ebp, edx;
        jg      L_TAPPING_GEM;
        je      L_HAMMER_GEM;
        
        mov edi, [ADDR_gemMatNormal2 + eax * 4];
        jmp returnAddress;

    L_HAMMER_GEM:
        mov edi, [ADDR_gemMatHammer2 + eax * 4];
        jmp returnAddress;

    L_TAPPING_GEM:
        mov edi, [ADDR_gemMatBattle2 + eax * 4];
        jmp returnAddress;
    }
}

__declspec(naked) void makeSpTapGemsNaked()
{
    const static uint32_t returnAddress = 0x004328A5;

    //.data:00A17868 g_gemMatHammerSp2 dd 0  
    __asm
    {
        mov edx, 2;
        cmp ebp, edx;
        mov edx, 1;
        jnz NORMAL_NOTE;

    TAP_NOTE:     
        mov eax, 3;
        mov     edi, [ADDR_gemMatBattle2 + eax * 4]; // g_gemMatHammerSp2[eax * 4]
        jmp returnAddress;

    NORMAL_NOTE:
        mov     edi, [eax * 4 + 0x00A1787C]; // g_gemMatHammerSp2[eax * 4]
        jmp returnAddress;
    }
}

void ApplyHack()
{
    gh3p::WriteJmp(cave1DetourAddress, &loadTappingFlagNaked);

    gh3p::WriteJmp(guitarInputLogicDetour1, &loadLastHitNaked);
    gh3p::WriteJmp(guitarInputLogicDetour2, &setCanTapStateNaked);
    gh3p::WriteJmp(chordHitCheckDetour, &chordHitCheckNaked);
    {
        uint8_t *jnz_to_CHORD_CHECK_FAILED = (uint8_t *)(0x00431E75); // test before was cmp hopo vs 1
        gh3p::ScopedUnprotect unprotect(jnz_to_CHORD_CHECK_FAILED, 2);
        *jnz_to_CHORD_CHECK_FAILED = (uint8_t)0x7C; //Replace jnz with jl
    };
    gh3p::WriteJmp(noteHitEndDetour2, &noteHitEndNaked);

    gh3p::WriteJmp(setStarGemAppearanceDetour, &makeStarTapGemsNaked);
    gh3p::WriteJmp(setNormalGemAppearanceDetour, &makeTapGemsNaked);
    gh3p::WriteJmp(setSpGemAppearanceDetour, &makeSpTapGemsNaked);

}
