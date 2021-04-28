#pragma once

#pragma warning(disable : 4996)

#include <Windows.h>
#include <iostream>

#include <vadefs.h>
#include <cstdarg>
#include <vector>

#include <fstream>

#pragma comment(lib, "lib/Release/libMinHook.x64.lib")
#include <MinHook.h>

namespace base
{
	inline HMODULE g_mModule = 0;
	inline bool g_Running = true;
}