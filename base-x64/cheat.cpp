#include "cheat.hpp"
#include "unity.hpp"
#include "hooks.hpp"
#include "script_mgr.hpp"
#include "fiber_pool.hpp"
#include "natives.hpp"
#include "gta/ped_factory.hpp"

#include "gui/menus/HomeGui.hpp"
#include "gui/Gui.hpp"

#include "UI/UIManager.hpp"
#include "crashlogger.hpp"

namespace base
{

	void Cheat::initialize()
	{
		g_CheatPath /= "UnityM";

		std::filesystem::create_directory(g_CheatPath);

		g_Logger = std::make_unique<log>();
		crashlogger::g_CrashLogger = std::make_unique<crashlogger::Crashlogger>();
		crashlogger::g_CrashLogger->initialize();
		g_Cheat = std::make_unique<Cheat>();

		g_Logger->initialize();
		

		g_Unity = std::make_unique<Unity>();
		g_Unity->init();
		g_Cheat->init();
		
		g_UiManager = std::make_unique<base::UserInterface::UIManager>();

		g_fiber_pool = std::make_unique<fiber_pool>(10);

		g_script_mgr->add_script(std::make_unique<script>(&Cheat::init_script));
		g_script_mgr->add_script(std::make_unique<script>(&fiber_pool::fiber_func));


		g_Logger->Info("Initialized!");

		while (g_Running)
		{
			if (GetKeyState(VK_END))
				g_Running = false;
			
			std::this_thread::sleep_for(500ms);
		}

		g_Cheat->shutdown();
		g_script_mgr->remove_all_scripts();

		g_fiber_pool.reset();

		g_Unity.reset();
		g_UiManager.reset();
		g_script_mgr.reset();
		g_native_invoker.reset();
		g_Logger->shutdown();
		g_Cheat.reset();
		crashlogger::g_CrashLogger->uninitialize();
		crashlogger::g_CrashLogger.reset();
		g_Logger.reset();

		FreeLibraryAndExitThread(base::g_mModule, 0);
	}

	void Cheat::init()
	{
		MH_Initialize();
		hooks::hook();
		MH_EnableHook(MH_ALL_HOOKS);
	}

	void Cheat::shutdown()
	{
		hooks::unhook();
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}

	void Cheat::tick()
	{
		auto pedFac = *g_Unity->m_ped_factory;

		if (pedFac != 0)
		{
			CPed* myPed = pedFac->m_local_ped;

			if (myPed != 0)
			{
				myPed->SetGodModeFlag(gui::g_HomeGuiConfig.m_GodMode);

				if (gui::g_HomeGuiConfig.m_NeverWanted == true)
				{

					myPed->SetPlayerWantedLevel(0);

				}
			}

			
		}
	}

	void Cheat::init_script()
	{
		while (g_Running)
		{

			g_Cheat->tick();
			gui::Tick();
			//g_UiManager->OnTick();
			gui::HomeGui::OnTick();

			script::get_current()->yield();
		}
	}

}