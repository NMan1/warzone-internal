#pragma once

extern "C" void _do_syscall();

template< typename ReturnType = void, typename... Args,
	typename T1 = void*, typename T2 = void*, typename T3 = void*, typename T4 = void* >
	ReturnType syscall(const unsigned __int64 Index, T1 A1 = { }, T2 A2 = { }, T3 A3 = { }, T4 A4 = { }, Args... Arguments)
{
	return reinterpret_cast<ReturnType(*)(T1, T2, T3, T4, unsigned __int64, unsigned __int64, Args...)>(_do_syscall)(
		A1, A2, A3, A4, Index, 0, Arguments... // Stack must be aligned to 16 byte boundary.
		);
}
