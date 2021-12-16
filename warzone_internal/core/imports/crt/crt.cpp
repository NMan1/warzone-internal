#include "crt.h"
#include <intrin.h>
#include "../syscall/syscall.h"

#pragma warning(disable:4146)

#define PAGE_READWRITE          0x04    
#define MEM_COMMIT              0x00001000  
#define MEM_RESERVE             0x00002000  
#define MEM_RELEASE             0x00008000  

void* __cdecl operator new(size_t size) {
	void* allocation = nullptr;
	const auto status = syscall< long >(0x18, (void*)-1, &allocation, 0, &size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	return allocation;
}

void operator delete(void* ptr) {
	const auto status = syscall< long >(0x1E, (void*)-1, &ptr, 0, MEM_RELEASE);
}

void* __cdecl operator new[](size_t n) {
	return operator new(n);
}

void  __cdecl operator delete[](void* p) {
	operator delete(p);
}

void  __cdecl operator delete[](void* p, size_t) {
	operator delete[](p);
}

void  __cdecl operator delete(void* p, size_t) {
	operator delete(p);
}

namespace crt {
	void* malloc(size_t size) {
		void* allocation = nullptr;
		const auto status = syscall< long >(0x18, (void*)-1, &allocation, 0, &size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		return allocation;
	}

	void free(void* ptr) {
		const auto status = syscall< long >(0x1E, (void*)-1, &ptr, 0, MEM_RELEASE);
	}

	float sqrtf(float _X) { 
		float x = _X;
		float y = 1;
		float e = 0.000001; /* e decides the accuracy level*/
		while (x - y > e) {
			x = (x + y) / 2;
			y = _X / x;
		}
		return x;
	}

	float sinf(float _X) { 
		const float a0 = +1.91059300966915117e-31f;
		const float a1 = +1.00086760103908896f;
		const float a2 = -1.21276126894734565e-2f;
		const float a3 = -1.38078780785773762e-1f;
		const float a4 = -2.67353392911981221e-2f;
		const float a5 = +2.08026600266304389e-2f;
		const float a6 = -3.03996055049204407e-3f;
		const float a7 = +1.38235642404333740e-4f;
		return a0 + _X * (a1 + _X * (a2 + _X * (a3 + _X * (a4 + _X * (a5 + _X * (a6 + _X * a7))))));
	}

	float cosf(float _X) { 
		const float a0 = 9.9995999154986614e-1f;
		const float a1 = 1.2548995793001028e-3f;
		const float a2 = -5.0648546280678015e-1f;
		const float a3 = 1.2942246466519995e-2f;
		const float a4 = 2.8668384702547972e-2f;
		const float a5 = 7.3726485210586547e-3f;
		const float a6 = -3.8510875386947414e-3f;
		const float a7 = 4.7196604604366623e-4f;
		const float a8 = -1.8776444013090451e-5f;
		return a0 + _X * (a1 + _X * (a2 + _X * (a3 + _X * (a4 + _X * (a5 + _X * (a6 + _X * (a7 + _X * a8)))))));
	}

	//float tanf(float _X) { return _mm_cvtss_f32(_mm_tan_ps(_mm_set_ss(_X))); }

	//float asinf(float _X) { return _mm_cvtss_f32(_mm_asin_ps(_mm_set_ss(_X))); }

	double asin_core(double x) {
		double x8, x4, x2;
		x2 = x * x;
		x4 = x2 * x2;
		x8 = x4 * x4;
		/* evaluate polynomial using a mix of Estrin's and Horner's scheme */
		return (((4.5334220547132049e-2 * x2 - 1.1226216762576600e-2) * x4 +
			(2.6334281471361822e-2 * x2 + 2.0596336163223834e-2)) * x8 +
			(3.0582043602875735e-2 * x2 + 4.4630538556294605e-2) * x4 +
			(7.5000364034134126e-2 * x2 + 1.6666666300567365e-1)) * x2 * x + x;
	}

	double acosf(double _X) { 
		double xa, t;
		xa = fabs(_X);
		/* arcsin(x) = pi/2 - 2 * arcsin (sqrt ((1-x) / 2))
		 * arccos(x) = pi/2 - arcsin(x)
		 * arccos(x) = 2 * arcsin (sqrt ((1-x) / 2))
		 */
		if (xa > 0.5625) {
			t = 2.0 * asin_core(sqrtf(0.5 * (1.0 - xa)));
		}
		else {
			t = 1.5707963267948966 - asin_core(xa);
		}
		/* arccos (-x) = pi - arccos(x) */
		return (_X < 0.0) ? (3.1415926535897932 - t) : t;
	}
	
	int floorf(float x) {
		x = (float)((int)x - ((x < 0.0f) ? 1 : 0));
		return (int)x;
	}

	int ceilf(float x) {
		if (x >= 0) {
			int i = (int)x;
			return (x > i) ? i + 1 : i;
		}
		else {
			int t = (int)x;
			float r = x - (float)t;
			return (r > 0.0f) ? t + 1 : t;
		}
	}

	double fmod(double x, double y) {
		union { double f; unsigned __int64 i; } ux = { x }, uy = { y };
		int ex = ux.i >> 52 & 0x7ff;
		int ey = uy.i >> 52 & 0x7ff;
		int sx = ux.i >> 63;
		unsigned __int64 i;

		/* in the followings uxi should be ux.i, but then gcc wrongly adds */
		/* float load/store to inner loops ruining performance and code size */
		unsigned __int64 uxi = ux.i;

		if (uy.i << 1 == 0 || (y != y) || ex == 0x7ff)
			return (x * y) / (x * y);
		if (uxi << 1 <= uy.i << 1) {
			if (uxi << 1 == uy.i << 1)
				return 0 * x;
			return x;
		}

		/* normalize x and y */
		if (!ex) {
			for (i = uxi << 12; i >> 63 == 0; ex--, i <<= 1);
			uxi <<= -ex + 1;
		}
		else {
			uxi &= -1ULL >> 12;
			uxi |= 1ULL << 52;
		}
		if (!ey) {
			for (i = uy.i << 12; i >> 63 == 0; ey--, i <<= 1);
			uy.i <<= -ey + 1;
		}
		else {
			uy.i &= -1ULL >> 12;
			uy.i |= 1ULL << 52;
		}

		/* x mod y */
		for (; ex > ey; ex--) {
			i = uxi - uy.i;
			if (i >> 63 == 0) {
				if (i == 0)
					return 0 * x;
				uxi = i;
			}
			uxi <<= 1;
		}
		i = uxi - uy.i;
		if (i >> 63 == 0) {
			if (i == 0)
				return 0 * x;
			uxi = i;
		}
		for (; uxi >> 52 == 0; uxi <<= 1, ex--);

		if (ex > 0) {
			uxi -= 1ULL << 52;
			uxi |= (unsigned __int64)ex << 52;
		}
		else {
			uxi >>= -ex + 1;
		}
		uxi |= (unsigned __int64)sx << 63;
		ux.i = uxi;
		return ux.f;
	}

	double pow(double x, int n) {
		double r = 1;
		int plus = n >= 0;
		n = (plus) ? n : -n;
		while (n > 0) {
			if ((n & 1) == 1)
				r *= x;
			n /= 2;
			x *= x;
		}
		return plus ? r : 1.0 / r;
	}

	double fabs(double x) {
		union { double f; unsigned __int64 i; } u = { x };
		u.i &= -1ULL / 2;
		return u.f;
	}

	int toupper(int c) {
		if (c >= 'a' && c <= 'z') return c - 'a' + 'A';
		return c;
	}

	int tolower(int c) {
		if (c >= 'A' && c <= 'Z') return c - 'A' + 'a';
		return c;
	}

	int towupper(int c) {
		if (c >= L'a' && c <= L'z') return c - L'a' + L'A';
		return c;
	}

	int towlower(int c) {
		if (c >= L'A' && c <= L'Z') return c - L'A' + L'a';
		return c;
	}

	char* _cslwr(char* x) {
		char* y = x;
		while (*y)
		{
			*y = crt::tolower(*y);
			y++;
		}
		return x;
	}

	char* _csupr(char* x) {
		char* y = x;
		while (*y)
		{
			*y = crt::tolower(*y);
			y++;
		}
		return x;
	}

	int strlen(const char* string) {
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}

	const char* strcpy(char* buffer, const char* string) {
		char* ret = buffer;
		while (*string) *buffer++ = *string++;
		*buffer = 0;
		return ret;
	}

	const char* strcpy(char* buffer, const wchar_t* string) {
		char* ret = buffer;
		while (*string) *buffer++ = char(*string++);
		*buffer = 0;
		return ret;
	}

	int strcmp(const char* cs, const char* ct) {
		if (cs && ct)
		{
			while (*cs == *ct)
			{
				if (*cs == 0 && *ct == 0) return 0;
				if (*cs == 0 || *ct == 0) break;
				cs++;
				ct++;
			}
			return *cs - *ct;
		}
		return -1;
	}

	int stricmp(const char* cs, const char* ct) {
		if (cs && ct)
		{
			while (crt::tolower(*cs) == crt::tolower(*ct))
			{
				if (*cs == 0 && *ct == 0) return 0;
				if (*cs == 0 || *ct == 0) break;
				cs++;
				ct++;
			}
			return crt::tolower(*cs) - crt::tolower(*ct);
		}
		return -1;
	}

	const char* strstr(char const* _Str, char const* _SubStr) {
		const char* bp = _SubStr;
		const char* back_pos;
		while (*_Str != 0 && _Str != 0 && _SubStr != 0)
		{
			back_pos = _Str;
			while (tolower(*back_pos++) == tolower(*_SubStr++))
			{
				if (*_SubStr == 0)
				{
					return (char*)(back_pos - strlen(bp));
				}
			}
			++_Str;
			_SubStr = bp;
		}
		return 0;
	}

	int strwlen(const wchar_t* string) {
		int cnt = 0;
		if (string)
		{
			for (; *string != 0; ++string) ++cnt;
		}
		return cnt;
	}

	const wchar_t* wstrstr(wchar_t const* _Str, wchar_t const* _SubStr) {
		const wchar_t* bp = _SubStr;
		const wchar_t* back_pos;
		while (*_Str != 0 && _Str != 0 && _SubStr != 0)
		{
			back_pos = _Str;
			while (towlower(*back_pos++) == towlower(*_SubStr++))
			{
				if (*_SubStr == 0)
				{
					return (wchar_t*)(back_pos - strwlen(bp));
				}
			}
			++_Str;
			_SubStr = bp;
		}
		return 0;
	}

	void* memcpy(void* dest, const void* src, unsigned __int64 count) {
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;
		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}
		return dest;
	}

	void* memset(void* dest, char c, unsigned __int64 len) {
		unsigned int i;
		unsigned int fill;
		unsigned int chunks = len / sizeof(fill);
		char* char_dest = (char*)dest;
		unsigned int* uint_dest = (unsigned int*)dest;
		fill = (c << 24) + (c << 16) + (c << 8) + c;

		for (i = len; i > chunks * sizeof(fill); i--) {
			char_dest[i - 1] = c;
		}

		for (i = chunks; i > 0; i--) {
			uint_dest[i - 1] = fill;
		}

		return dest;
	}

	void* memccpy(void* to, const void* from, int c, unsigned __int64 count) {
		char t;
		unsigned __int64 i;
		char* dst = (char*)to;
		const char* src = (const char*)from;
		for (i = 0; i < count; i++)
		{
			dst[i] = t = src[i];
			if (t == 0) break;
			if (t == c) return &dst[i + 1];
		}
		return 0;
	}

	void* memchr(const void* s, int c, unsigned __int64 n) {
		if (n)
		{
			const char* p = (const char*)s;
			do
			{
				if (*p++ == c) return (void*)(p - 1);
			} while (--n != 0);
		}
		return 0;
	}

	int memcmp(const void* s1, const void* s2, unsigned __int64 n) {
		if (n != 0)
		{
			const unsigned char* p1 = (unsigned char*)s1, * p2 = (unsigned char*)s2;
			do
			{
				if (*p1++ != *p2++) return (*--p1 - *--p2);
			} while (--n != 0);
		}
		return 0;
	}

	int memicmp(const void* s1, const void* s2, unsigned __int64 n) {
		if (n != 0)
		{
			const unsigned char* p1 = (unsigned char*)s1, * p2 = (unsigned char*)s2;
			do
			{
				if (toupper(*p1) != toupper(*p2)) return (*p1 - *p2);
				p1++;
				p2++;
			} while (--n != 0);
		}
		return 0;
	}

	void* memmove(void* dest, const void* src, unsigned __int64 count) {
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;
		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}
		return dest;
	}
}
