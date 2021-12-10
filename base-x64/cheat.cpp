#include "cheat.hpp"

namespace base
{

	void Cheat::initialize()
	{
		g_Logger = std::make_unique<log>();
		g_Cheat = std::make_unique<Cheat>();

		g_Logger->initialize();
		g_Cheat->init();

		g_Logger->Info("Initialized!");

		while (g_Running)
		{
			if (GetKeyState(VK_END) & 1)
			{	g_Running = false;	}
		}

		g_Cheat->shutdown();
		g_Logger->shutdown();
		g_Cheat.reset();
		g_Logger.reset();

		CloseHandle(base::g_mThreadHandle);
		FreeLibraryAndExitThread(base::g_mModule, 0);
	}

	void Cheat::init()
	{
		MH_Initialize();

		MH_EnableHook(MH_ALL_HOOKS);
	}

	void Cheat::shutdown()
	{
		MH_DisableHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}

}