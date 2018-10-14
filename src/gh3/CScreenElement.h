#pragma once
#include <stdint.h>
#include "gh3\CObject.h"
#include "gh3\QbStruct.h"

namespace GH3
{
    /* List of classes that derive from CScreenElement:
        Front::CBaseMenu            
        Front::CBaseScrollingMenu   
        Front::CContainerElement    
        Front::CElement3d           
        Front::CHMenu               
        Front::CHScrollingMenu      
        Front::CMovieElement                                                                                                                                                                                             
        Front::CTextBlockElement    
        Front::CTextElement         
        Front::CVMenu               
        Front::CVScrollingMenu      
        Front::CViewportElement     
        Front::CWindowElement       
     */

    /// Any element that appears on the screen. This should technically live in the "Front" namespace.
	class GH3P_API CScreenElement : CObject
	{
	public:
		struct ElementBox
		{
			CScreenElement *left;
			CScreenElement *right;
			uint32_t data;
		};

		struct ScrEleStructD4
		{
			float float0;
			uint32_t dword4;
			uint32_t dword8;
			uint32_t dwordC;
			float float10;
			float float14;
			float float18;
			float float1C;
			float float20;
			uint16_t word24;
			uint16_t word26;
			float widthScaleMod1;
			float heightScaleMod1;
			float widthScaleMod2;
			float heightScaleMod2;
			uint32_t dword38;
			uint8_t byte3C;
			uint8_t byte3D;
			uint8_t byte3E;
			uint8_t byte3F;
		};


		struct ScrEleStruct1C4
		{
			uint8_t byte0;
			uint32_t dword4;
			uint32_t dword8;
			uint32_t dwordC;
			uint32_t dword10;
			uint32_t dword14;
			ScrEleStruct1C4 *next;
		};


		struct ScrEleStruct1E4
		{
			ScrEleStruct1E4 *prev;
			ScrEleStruct1E4 *next;
			uint32_t data;
		};

		struct __declspec(align(4)) VFTable
		{
			CScreenElement * (__thiscall *dtor)(CScreenElement *this_, bool onHeap);
			int (__thiscall *method1)(CScreenElement *this_);
			int (__thiscall *method2)(CScreenElement *this_, GH3::QbStruct *);
			int (__thiscall *method3)(CScreenElement *this_);
			int (__thiscall *method4)(CScreenElement *this_);
			int (__thiscall *method5)(CScreenElement *this_);
			int (__thiscall *method6)(CScreenElement *this_);
			int (__thiscall *method7)(CScreenElement *this_);
			int (__thiscall *method8)(CScreenElement *this_);
			int (__thiscall *method9)(CScreenElement *this_);
			int (__thiscall *method10)(CScreenElement *this_);
			int (__thiscall *method11)(CScreenElement *this_);
			int (__thiscall *method12)(CScreenElement *this_);
			int (__thiscall *method13)(CScreenElement *this_, void *);
		};


		ElementBox box64;
		ElementBox box70;
		ElementBox box7C;
		ElementBox box88;
		ScrEleStructD4 struct94;
		ScrEleStructD4 structD4;
		ScrEleStructD4 struct114;
		ScrEleStructD4 struct154;
		float addedTo1;
		float unkFloat_198;
		float unkFloat_19C;
		float unkFloat_1A0;
		float Width;
		float Height;
		uint8_t byte1AC;
		uint8_t byte1AD;
		uint8_t byte1AE;
		uint8_t byte1AF;
		float unk1B0_4_1;
		float unk1B4_0_0;
		uint32_t field_1B8;
		uint32_t field_1BC;
		uint32_t field_1C0;
		ScrEleStruct1C4 struct1C4;
		uint32_t field_1E0;
		ScrEleStruct1E4 struct1E4;
	};
}