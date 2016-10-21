GH3+
====

GH3+ is a work in progress plugin system for Guitar Hero III written in C++.

Latest release
--------------

The most recent release is v0.5. This release contains:
- core
- AlwaysDoubleNotes
- ForceStrumNotes
- ForceTapeNotes
- GemShuffle
- KeyHijacker (Buggy Speedhack)
- MirrorMode
- NoteLimitFix
- TapHopoChord
- TextureHelper
- TiltFix
- WhammyFix

Fixes from the latest version include:
- Statically linked DLLs in release mode by default. This should fix
- Fixed HOPO bug in MirrorMode plugin. Rewritten from scratch.
- Fixed HOPO bug in DoubleNotes plugin. Proper interaction with MirrorMode.

The largest changes from the latest version are:
- Removed std collection methods from the public GH3P APIs to prevent potential problems
- Fixed all warnings
- Renamed import/export macro to `GH3P_API` and moved to [GH3Plus.h].