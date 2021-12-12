#include "crt.h"
#include <intrin.h>
#include <winternl.h>

long NtAllocateVirtualMemory(
	void*    ProcessHandle,
	void**   BaseAddress,
	unsigned long* ZeroBits,
	size_t*   RegionSize,
	unsigned long     AllocationType,
	unsigned long      Protect
	);

void* __cdecl operator new(size_t size) {
	return nullptr;
	//return static_cast<void*>(j_malloc(size));
}

void operator delete(void* p) {
	//if (p != NULL)
	//	j_free(p);
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
	float sqrtf(float _X) { return _mm_cvtss_f32(_mm_sqrt_ss(_mm_set_ss(_X))); }

	float expf(float _X) { return _mm_cvtss_f32(_mm_exp_ps(_mm_set_ss(_X))); }

	float sinf(float _X) { return _mm_cvtss_f32(_mm_sin_ps(_mm_set_ss(_X))); }

	float cosf(float _X) { return _mm_cvtss_f32(_mm_cos_ps(_mm_set_ss(_X))); }

	float tanf(float _X) { return _mm_cvtss_f32(_mm_tan_ps(_mm_set_ss(_X))); }

	float asinf(float _X) { return _mm_cvtss_f32(_mm_asin_ps(_mm_set_ss(_X))); }
	
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

	int strlen(const wchar_t* string) {
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
					return (wchar_t*)(back_pos - strlen(bp));
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