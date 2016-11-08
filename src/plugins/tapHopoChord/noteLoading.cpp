#include "core\Patcher.h"
#include "gh3\GH3Keys.h"
#include "gh3\GH3GlobalAddresses.h"
#include "tapHopoChord.h"

//GH3 functions
static const void * const QbKvPairListGetByte = (void *)0x0040A9B0;

//Using global variables as a substitution for stack space
uint32_t g_openLength = 0;
uint32_t g_hopoFlag = 0;


// Open notes - increase fret array
// During loading, mask GRYBO bits if open flag is set























bool IsSingleNote(FretMask fretMask)
{
	return (fretMask == FretMask::GREEN ||
		fretMask == FretMask::RED ||
		fretMask == FretMask::YELLOW ||
		fretMask == FretMask::BLUE ||
		fretMask == FretMask::ORANGE ||
		fretMask == FretMask::OPEN);
}

FretMask __fastcall GetFretmaskFromTrackArray(GH3::QbArray *trackArray, int currentNote)
{
	int size; 
	int *noteTrack; 
	uint32_t result; 
	int realFretMask;

	size = trackArray->Length;
	noteTrack = trackArray->A;                  
												  
	result = 0;             

	realFretMask = trackArray->Get(currentNote + 2);

	if (realFretMask & QbFretMask::QbOpen)
		return FretMask::OPEN;

	if (realFretMask & QbFretMask::QbGreen)
		result = FretMask::GREEN;                 
													
	if (realFretMask & QbFretMask::QbRed)
		result |= FretMask::RED;                  
													
	if (realFretMask & QbFretMask::QbYellow)
		result |= FretMask::YELLOW;                           
													
	if (realFretMask & QbFretMask::QbBlue)
		result |= FretMask::BLUE;                              
													 
	if (realFretMask & QbFretMask::QbOrange)
		result |= FretMask::ORANGE;                           
												
	return (FretMask)result;
}

FretMask __fastcall GetFretmaskFromNoteQbArray(GH3::QbArray *noteArr)
{
	

	//if (size != 1)
	//	fret = noteArr->arr[8];
	//if (fret > 0)
	//	return FretMask::OPEN;

	if (noteArr->Get(8))
		return FretMask::OPEN;

	uint32_t fretMask = 0;
		
	if (noteArr->Get(1))
		return FretMask::GREEN;

	if (noteArr->Get(2))
		return FretMask::RED;

	if (noteArr->Get(3))
		return FretMask::YELLOW;

	if (noteArr->Get(4))
		return FretMask::BLUE;

	if (noteArr->Get(5))
		return FretMask::ORANGE;


	//if (size != 1)
	//	fret = noteArr->arr[1];
	//if (fret > 0)
	//	fretMask |= FretMask::GREEN;
	//
	//if (size != 1)
	//	fret = noteArr->arr[2];
	//if (fret > 0)
	//	fretMask |= FretMask::RED;
	//
	//if (size != 1)
	//	fret = noteArr->arr[3];
	//if (fret > 0)
	//	fretMask |= FretMask::YELLOW;
	//
	//if (size != 1)
	//	fret = noteArr->arr[4];
	//if (fret > 0)
	//	fretMask |= FretMask::BLUE;
	//
	//if (size != 1)
	//	fret = noteArr->arr[5];
	//if (fret > 0)
	//	fretMask |= FretMask::ORANGE;

	return (FretMask)fretMask;
}




// Injected into the code loading an individual note
static void * const hopoFlagDetour = (void *)0x0041D22E;
__declspec(naked) void loadTappingFlagNaked()
{
	static const int frameOffset = 0x48 + 0x4 * 8; //push an addition 4 for each offset register

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

		mov      eax, [esp + 0x78]; //[esp + arg_C]
		mov      edx, [eax + 4];
		mov      eax, [eax + 8];
		mov      edi, eax;
		mov      ecx, eax;
		cmp      edx, 1;
		jz       branch_a;
		mov      edi, [eax + esi * 4 + 4];
		mov      ecx, [eax + esi * 4 + 8];

		branch_a:
		sar      ecx, 6;
		and		 ecx, 1;
		cmp      ecx, 1;
		jnz      branch_b;
		mov      edi, 2;
		mov		 [esp + 0x40], edi; //[esp + hopoRaw]

		branch_b :

		popad;

		mov     eax, KEY_PLAYER; //Existing code we pushed out of the way to jump to this cave
		push    eax;
		call    QbKvPairListGetByte;

		jmp     returnAddress; //Return to normal execution

	};
}




void __stdcall LoadOpenHopoTappingBits(GH3::QbArray *noteQbArr, QbArray *placedNotesQbArr, int currentNote, int noteIndex, float hammerOnTolerance)
{
	//noteQbArr has a repeating sequence of [offset in ms, length in ms, fretmask]
	//placedNotesQbArr is an array of QbArrays

	int sustainLength = 0;
	int openLength = 0;
	HopoFlag hopoFlag; //Actual flag that will be set on the note placed on the highway
	HopoFlag hopoRaw = HopoFlag::STRUMMED; //Calculated hopo value
	bool hopoFlip = false;
	bool tappingFlag = false;

	QbArray *prevNoteQbArray;

	FretMask fretMask;
	FretMask prevFretMask;
	FretMask currentFretMask;

	int noteQbArrSize = noteQbArr->size;
	int prevNoteTime;
	int noteTime;

	if (noteQbArrSize != 1)
	{
		sustainLength = noteQbArr->arr[currentNote + 1]; 
		fretMask = static_cast<FretMask>(noteQbArr->arr[currentNote + 2]);

		if (sustainLength * (fretMask & QbFretMask::QbHopoflip) != 0)
			hopoFlip = true;
	
		if (sustainLength * (fretMask & QbFretMask::QbTapping) != 0)
			tappingFlag = true;

		openLength = sustainLength * ((fretMask >> 7) & 1);
	}

	if (noteIndex > 0)
	{
		if (placedNotesQbArr->size == 1)
			prevNoteQbArray = reinterpret_cast<GH3::QbArray *>(placedNotesQbArr->arr);
		else
			prevNoteQbArray = reinterpret_cast<GH3::QbArray *>(placedNotesQbArr->arr[noteIndex - 1]);

		prevFretMask = GetFretmaskFromNoteQbArray(prevNoteQbArray);
		currentFretMask = GetFretmaskFromTrackArray(noteQbArr, currentNote);

		if (prevNoteQbArray->size == 1)
			prevNoteTime = reinterpret_cast<int>(prevNoteQbArray->arr);
		else
			prevNoteTime = prevNoteQbArray->arr[0];

		if (noteQbArr->size == 1)
			noteTime = reinterpret_cast<int>(noteQbArr->arr);
		else
			noteTime = noteQbArr->arr[currentNote];

		if (hammerOnTolerance >= ((float)noteTime - (float)prevNoteTime)
			&& IsSingleNote(currentFretMask)
			&& currentFretMask != prevFretMask)
		{
			hopoRaw = HopoFlag::HOPO;
		}
	}

	hopoFlag = static_cast<HopoFlag>(hopoFlip ^ hopoRaw);

	if (tappingFlag)
		hopoFlag = HopoFlag::TAPPING;

	g_hopoFlag = hopoFlag;
	g_openLength = openLength;
}

static void * const LoadHopoFlagDetour = (void *)0x0041D14C;
void __declspec(naked) LoadOpenHopoTappingBitsNaked()
{
	static const unsigned int returnAddress = 0x0041D239;

	__asm
	{
		push	[esp + 40h]; // hammeronTolerance
		push	[esp + (30h + 04h)]; // noteIndex
		push	[esp + (54h + 08h)]; // currentNote
		push	[esp + (34h + 0Ch)]; // placedNotesQbArr
		push	[esp + (58h + 10h)]; // noteQbArr
		call	LoadOpenHopoTappingBits;

		mov		eax, g_hopoFlag;
		mov		[esp + 20h], eax;

		mov     ecx, [esp + 4Ch] //list
		mov     eax, KEY_PLAYER; //Existing code we pushed out of the way to jump to this cave
		push    eax;
		call    QbKvPairListGetByte;
		jmp     returnAddress; //Return to normal execution
	}
}





int __fastcall CreateNoteWithOpenImpl(GH3::QbArray *noteArray, int noteIndex, int noteTime, int greenLength, int redLength, int yellowLength, int blueLength, int orangeLength, int hopoFlag, int nextNoteTime)
{
	QbArray *note;

	if (noteArray->size == 1)
		note = (QbArray *)noteArray->arr;
	else
		note = (QbArray *)noteArray->arr[noteIndex];// 
													// 
	note->SetValue(0, noteTime);
	note->SetValue(1, greenLength);
	note->SetValue(2, redLength);
	note->SetValue(3, yellowLength);
	note->SetValue(4, blueLength);
	note->SetValue(5, orangeLength);
	note->SetValue(6, hopoFlag);
	note->SetValue(7, nextNoteTime);
	note->SetValue(8, g_openLength);

	return 1;
}

void *CreateNote = (void *)0x0041AB60;
int __declspec(naked) CreateNoteWithOpen()
{
	__asm
	{
		mov edx, ecx;
		mov ecx, eax;
		jmp CreateNoteWithOpenImpl;
	}
}











bool TryApplyNoteLoadingPatches()
{


	return (g_patcher.WriteInt8((void *)0x0041AAD6, 9) && // Allocate an extra slot for open notes in the open note structure (normally 8)
			g_patcher.WriteJmp(hopoFlagDetour, LoadOpenHopoTappingBitsNaked) &&

			g_patcher.WriteJmp(CreateNote, CreateNoteWithOpen) && //Substitute our own CreateNote function that handles open notes
			g_patcher.WriteNOPs((void *)0x0041D452, 3) && //NOP out the "sub esp 20h" following any CreateNote call since we are cleaning up the stack ourselves (effectively changing the calling convention)
			g_patcher.WriteNOPs((void *)0x0041AD97, 3) &&
			g_patcher.WriteInt8((void *)0x0041D451, 0x28)); //Since the stack will no longer be off by 0x20 we need to fix anything that grabs a stack variable before it is correct);
}
