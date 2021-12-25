#include "features.h"
#include "../game.h"
#include "../offsets.h"

namespace features {
	void no_recoil() {
		unsigned __int64 r12 = game::client_info;
		r12 += offsets::recoil;
		unsigned __int64 rsi = r12 + 0x4;
		DWORD edx = *(unsigned __int64*)(r12 + 0xC);
		DWORD ecx = (DWORD)r12;
		ecx ^= edx;
		DWORD eax = (DWORD)((unsigned __int64)ecx + 0x2);
		eax *= ecx;
		ecx = (DWORD)rsi;
		ecx ^= edx;
		DWORD udZero = eax;
		//left, right
		eax = (DWORD)((unsigned __int64)ecx + 0x2);
		eax *= ecx;
		DWORD lrZero = eax;
		*(DWORD*)(r12) = udZero;
		*(DWORD*)(rsi) = lrZero;
	}
}