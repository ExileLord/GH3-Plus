#pragma once

namespace GH3
{
    // Various memory allocation calls used by classes in the game. Since the game's memory management is different than ours, any game class created
    // that uses dynamic memory allocation should be using these calls so that the game cleans them up properly.

	///used for struct items?
	static void * (__cdecl * const qbItemMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x00437410); 

	///Used for qbstructs
	static void * (__cdecl * const qbMalloc)(size_t size, int unused) = reinterpret_cast<void * (__cdecl *)(size_t size, int unused)>(0x00402370);
}