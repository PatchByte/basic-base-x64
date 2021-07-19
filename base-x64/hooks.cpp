#include "hooks.hpp"
#include "logger.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "unity.hpp"
#include "gui/Gui.hpp"
#include "script_mgr.hpp"
#include "Memory.hpp"
#include "gta/net_game_event.hpp"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace base
{

	static PVOID PresentHookOrig = 0;
	static bool initialized = 0;
	static ID3D11RenderTargetView* mainRenderTargetView = 0;
	bool guiOpend = false;

	static DXGI_SWAP_CHAIN_DESC desc;
	static ID3D11Device* device;
	static ID3D11DeviceContext* context;

	long base::hooks::CoreHooks::PresentHook(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		__try {
			if (initialized == false)
			{
				pSwapChain->GetDesc(&desc);
				pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&device);
				device->GetImmediateContext(&context);
				

				ImGui::CreateContext();
				ImGuiIO& io = ImGui::GetIO();
				io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

				ImGui_ImplDX11_Init(device, context);
				ImGui_ImplWin32_Init(g_Unity->m_GameWindow);
				

				base::gui::Initialize();

				initialized = true;
			}

			if (guiOpend)
			{
				ImGui::GetIO().MouseDrawCursor = true;
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
			else
			{
				ImGui::GetIO().MouseDrawCursor = false;
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			}


			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();


			if (guiOpend)
			{
				base::gui::Render();
			}

			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			//context->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
			//ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{ }
		return static_cast<decltype(&CoreHooks::PresentHook)>(PresentHookOrig)(pSwapChain, SyncInterval, Flags);
	}

	static PVOID SwapchainResizeBuffersOrig = 0;
	HRESULT hooks::CoreHooks::SwapchainResizeBuffers(IDXGISwapChain* this_, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT swapchain_flags)
	{
		__try {
			ImGui_ImplDX11_InvalidateDeviceObjects();

			auto res = static_cast<decltype(&SwapchainResizeBuffers)>(SwapchainResizeBuffersOrig)(this_, buffer_count, width, height, new_format, swapchain_flags);

			ImGui_ImplDX11_CreateDeviceObjects();

			return res;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
		}
		auto res = static_cast<decltype(&SwapchainResizeBuffers)>(SwapchainResizeBuffersOrig)(this_, buffer_count, width, height, new_format, swapchain_flags);
		return res;
	}

	static PVOID WndProcOrig = 0;
	LRESULT base::hooks::CoreHooks::WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_KEYUP && wParam == VK_INSERT)
			guiOpend ^= true;

		if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;

		if (guiOpend)
		{
			return true;
		}

		return CallWindowProc((WNDPROC)WndProcOrig, hWnd, uMsg, wParam, lParam);
		//return static_cast<decltype(&WndProc)>(WndProcOrig)(hWnd, uMsg, wParam, lParam);
	}

	static PVOID ConvertThreadToFiberOrig = 0;
	void* base::hooks::CoreHooks::ConvertThreadToFiber(void* thread)
	{
		
			if (IsThreadAFiber())
			{
				return GetCurrentFiber();
			}
		
		return static_cast<decltype(&ConvertThreadToFiber)>(ConvertThreadToFiberOrig)(thread);
	}

	static PVOID run_script_threadsOrig = 0;
	bool base::hooks::CoreHooks::run_script_threads(std::uint32_t ops_to_execute)
	{
		if (g_Running)
		{
			g_script_mgr->tick();
		}

		return static_cast<decltype(&run_script_threads)>(run_script_threadsOrig)(ops_to_execute);
	}

	static PVOID OnPLayerJoinOrig = 0;
	static PVOID OnPLayerJoinLeft = 0;

	void base::hooks::NetworkHooks::OnPlayerJoin(void* t, CNetGamePlayer* pPlayer)
	{
		static_cast<decltype(&OnPlayerJoin)>(OnPLayerJoinOrig)(t, pPlayer);
		
		auto m_ip = pPlayer->get_net_data()->m_online_ip;

		g_Logger->Info("[+] %s [%u] [%i.%i.%i.%i]", pPlayer->GetName(), pPlayer->get_net_data()->m_rockstar_id, m_ip.m_field1, m_ip.m_field2, m_ip.m_field3, m_ip.m_field4);

		
	}

	void base::hooks::NetworkHooks::OnPlayerLeft(void* t, CNetGamePlayer* pPlayer)
	{
		static_cast<decltype(&OnPlayerLeft)>(OnPLayerJoinLeft)(t, pPlayer);
		
		auto m_ip = pPlayer->get_net_data()->m_online_ip;

		g_Logger->Info("[-] %s [%u] [%i.%i.%i.%i]", pPlayer->GetName(), pPlayer->get_net_data()->m_rockstar_id, m_ip.m_field1, m_ip.m_field2, m_ip.m_field3, m_ip.m_field4);

	}

	static PVOID ScriptedGameEventHandlerOrig = 0;
	bool base::hooks::NetworkHooks::ScriptedGameEventHandler(CScriptedGameEvent* this_, CNetGamePlayer* source_player, CNetGamePlayer* target_player)
	{
		if (this_->m_args_size <= 1)
		{
			return true;
		}

		int origId = source_player->GetPlayerId();
		int scriptId = this_->m_args[1];

		if (source_player == 0)
		{
			return true;
		}

		if (origId != scriptId)
		{

			return true;
		}

		return static_cast<decltype(&ScriptedGameEventHandler)>(ScriptedGameEventHandlerOrig)(this_, source_player, target_player);
	}

	void base::hooks::hook()
	{
		static PVOID m_PlayerHasJoined = Signature("48 8B C4 48 89 58 ? 48 89 68 ? 48 89 70 ? 48 89 78 ? 41 56 48 83 EC 20 48 8B 99 ? ? ? ? 48 8B 02 48 8B E9").Scan().As<PVOID>();
		static PVOID m_PlayerHasLeft = Signature("48 89 5C 24 ? 57 48 83 EC 20 48 8B FA 48 8B D9 E8 ? ? ? ? 45 33 D2").Scan().Add(0x10).Add(1).Rip().As<PVOID>();


		assert_bool("KIERO::INIT", kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success);

		WndProcOrig = (PVOID) SetWindowLongPtr(g_Unity->m_GameWindow, GWLP_WNDPROC, (LONG_PTR)CoreHooks::WndProc);

		kiero::bind(8, &PresentHookOrig, CoreHooks::PresentHook);
		kiero::bind(13, &SwapchainResizeBuffersOrig, CoreHooks::SwapchainResizeBuffers);


		assert_ptr("oh oh! [1]", g_Unity->m_convert_thread_to_fiber);
		MH_CreateHook(g_Unity->m_convert_thread_to_fiber, CoreHooks::ConvertThreadToFiber, &ConvertThreadToFiberOrig);
		MH_EnableHook(g_Unity->m_convert_thread_to_fiber);

		MH_CreateHook(g_Unity->m_run_script_threads, CoreHooks::run_script_threads, &run_script_threadsOrig);
		MH_EnableHook(g_Unity->m_run_script_threads);

		MH_CreateHook(m_PlayerHasJoined, NetworkHooks::OnPlayerJoin, &OnPLayerJoinOrig);
		MH_CreateHook(m_PlayerHasLeft, NetworkHooks::OnPlayerLeft, &OnPLayerJoinLeft);
	}

	void base::hooks::unhook()
	{
		SetWindowLongPtr(g_Unity->m_GameWindow, GWLP_WNDPROC, (LONG_PTR)WndProcOrig);

		kiero::unbind(8);
		kiero::unbind(13);
		kiero::shutdown();

		base::gui::Uninitialize();
	}

	
}