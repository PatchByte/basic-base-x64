#pragma once
#include "common.hpp"
#include "gta/player.hpp"
namespace base
{

	namespace hooks
	{

		struct CoreHooks
		{
			static bool run_script_threads(std::uint32_t ops_to_execute);
			static HRESULT PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
			static HRESULT SwapchainResizeBuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags);
			static LRESULT WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			static void* ConvertThreadToFiber(void* thread);

		};

		struct NetworkHooks
		{

			static void OnPlayerJoin(void*, CNetGamePlayer*);
			static void OnPlayerLeft(void*, CNetGamePlayer*);

			static bool ScriptedGameEventHandler(CScriptedGameEvent* this_, CNetGamePlayer* source_player, CNetGamePlayer* target_player);

		};

		void hook();
		void unhook();

		
	}

	extern bool guiOpend;

}