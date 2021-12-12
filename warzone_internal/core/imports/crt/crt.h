#pragma once

void* __cdecl operator new(size_t size);

void operator delete(void* p);

void* __cdecl operator new[](size_t n);

void  __cdecl operator delete[](void* p);

void  __cdecl operator delete[](void* p, size_t);

void  __cdecl operator delete(void* p, size_t);

namespace crt {
	float sqrtf(float _X);

	float expf(float _X);

	float sinf(float _X);

	float cosf(float _X);

	float tanf(float _X);

	float asinf(float _X);

	int toupper(int c);

	int tolower(int c);

	int towupper(int c);

	int towlower(int c);

	char* _cslwr(char* x);

	char* _csupr(char* x);

	int strlen(const char* string);

	const char* strcpy(char* buffer, const char* string);

	const char* strcpy(char* buffer, const wchar_t* string);

	int strcmp(const char* cs, const char* ct);

	int stricmp(const char* cs, const char* ct);

	const char* strstr(char const* _Str, char const* _SubStr);

	int strlen(const wchar_t* string);

	const wchar_t* wstrstr(wchar_t const* _Str, wchar_t const* _SubStr);

	void* memcpy(void* dest, const void* src, unsigned __int64 count);

	void* memset(void* dest, char c, unsigned __int64 len);

	void* memccpy(void* to, const void* from, int c, unsigned __int64 count);

	void* memchr(const void* s, int c, unsigned __int64 n);

	int memcmp(const void* s1, const void* s2, unsigned __int64 n);

	int memicmp(const void* s1, const void* s2, unsigned __int64 n);

	void* memmove(void* dest, const void* src, unsigned __int64 count);
}