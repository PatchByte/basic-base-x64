#pragma once

#pragma warning(disable : 4996)

#define NOMINMAX

#include <Windows.h>
#include <iostream>

#include <vadefs.h>
#include <cstdarg>

#include <string>
#include <fstream>

#pragma comment(lib, "lib/Release/libMinHook.x64.lib")
#include <MinHook.h>

#include "../kiero/kiero.h"

#include <d3d11_1.h>

#include <timeapi.h>
#pragma comment(lib, "Winmm.lib")

#include <functional>
#include <optional>
#include <chrono>
#include <mutex>

#include <unordered_map>
#include <vector>
#include <map>
#include <stack>

using namespace std::chrono_literals;

#define assert_ptr(y,x) if(x==0){g_Logger->Error("%s -> ASSERT PTR FAIL!",y);}
#define assert_bool(y,x) if(x!=1){g_Logger->Error("%s -> ASSERT BOOL FAIL!",y);}
#define script_mgr_add_script(x) std::make_unique<script>(&x);

#define RModel(x) while (!STREAMING::HAS_MODEL_LOADED(x)) \
{ \
	STREAMING::REQUEST_MODEL(x); \
	script::get_current()->yield(); \
} \
;

#include <filesystem>

namespace base
{
	inline HMODULE g_mModule = 0;
	inline bool g_Running = true;
	inline std::filesystem::path g_CheatPath = std::getenv("appdata");
}

struct Color
{
	int r, g, b, a;
};