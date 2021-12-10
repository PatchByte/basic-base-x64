#pragma once

#pragma warning(disable : 4996)

#include <Windows.h>
#include <iostream>

#include <vadefs.h>
#include <cstdarg>
#include <vector>

#include <fstream>

#ifdef _WIN64
#pragma comment(lib, "lib/Release/libMinHook.x64.lib")
#else
#pragma comment(lib, "lib/Release/libMinHook.x32.lib")
#endif
#include <MinHook.h>

namespace base
{
	inline HMODULE g_mModule = NULL;

	inline HANDLE g_mThreadHandle = NULL;

	inline bool g_Running = true;
}