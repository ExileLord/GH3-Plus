#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"

#include "makeMeGood.h"
#include <random>

//static void * guitarInputDetour = (void *)0x00431D65;  ////0x00431CCE; //0x0043190D;
//static void * guitarInputDetour2 = (void *)0x0043226B; // 0x004320EC; //overstrum

static void * const fruityDetour = (void *)0x00432887; //0x004327D2; // 0x00432733;

static void * const overstrumDetour1 = (void *)0x004320F0; //overstrum

static uint32_t g_randomIndex;

static GH3P::Patcher g_patcher = GH3P::Patcher(__FILE__);

void randomizeIndex()
{
	g_randomIndex = rand() % 5;
}

__declspec (naked) void fruityNaked()
{
	static const uint32_t returnAddress = 0x0043288F;

	__asm
	{
		pushad;
		call	randomizeIndex;
		popad;
		mov		eax, g_randomIndex;
		//mov		[esp + 94h], eax;

		test    ecx, ecx
		movss	[esp + 34h], xmm0

		jmp     returnAddress;
	}
}



void ApplyHack()
{
    //gh3p::WriteJmp(guitarInputDetour,  (void *)0x00431F5D);
	//gh3p::WriteJmp(guitarInputDetour2, (void *)0x004320D3); 

	//gh3p::WriteJmp(overstrumDetour1, (void *)0x004320FD); remove overstrums

	g_patcher.WriteJmp(fruityDetour, (void *)fruityNaked); 

}