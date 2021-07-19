#include "unity.hpp"
#include "Memory.hpp"
#include "logger.hpp"
#include "invoker.hpp"
#include "script_mgr.hpp"

#include "gta/script_thread.hpp"

namespace base
{

	void base::Unity::init()
	{

		__int64 TickCountStart = GetTickCount64();

		m_GameWindow = FindWindowW(L"grcWindow", nullptr);

		{
			MemoryHandle temp = Signature("48 83 ec 28 33 c0 38 05 ? ? ? ? 74 0a 83 f9 1f 77 05").Scan();

			m_is_session_started = temp.Add(6 + 2).Rip().As<bool*>();
			assert_ptr("IsSessionStarted", m_is_session_started);

			m_get_player_by_id = temp.As<get_player_by_id>();
			assert_ptr("GetPlayerById", m_get_player_by_id);
		}

		m_ped_factory = Signature("48 8B 05 ? ? ? ? 48 8B 48 08 48 85 C9 74 52 8B 81").Scan().Add(3).Rip().As<CPedFactory**>();
		assert_ptr("CPedFactory -> Address", m_ped_factory);

		{
			MemoryHandle m_Temp = Signature("48 8D 0D ? ? ? ? 48 8B 14 FA E8 ? ? ? ? 48 85 C0 75 0A").Scan();

			m_native_registration_table = m_Temp.Add(3).Rip().As<rage::scrNativeRegistrationTable*>();
			m_get_native_handler = m_Temp.Add(12).Rip().As<get_native_handler_t>();
			m_fix_vectors = Signature("83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA").Scan().As<fix_vectors_t>();

			assert_ptr("FixVectors", m_fix_vectors);
		}

		{
			MemoryHandle m_Temp = Signature("45 33 F6 8B E9 85 C9 B8").Scan();

			m_script_threads = m_Temp.Sub(4).Rip().Sub(8).As<decltype(m_script_threads)>();
			m_run_script_threads = m_Temp.Sub(0x1F).As<PVOID>();

			assert_ptr("Script Threads", m_script_threads);
			assert_ptr("Run Script Threads", m_run_script_threads);
		}

		m_script_program_table = Signature("44 8B 0D ? ? ? ? 4C 8B 1D ? ? ? ? 48 8B 1D ? ? ? ? 41 83 F8 FF 74 3F 49 63 C0 42 0F B6 0C 18 81 E1").Scan().Add(17).Rip().As<decltype(m_script_program_table)>();
		assert_ptr("ScriptProgramTable", m_script_program_table);

		m_script_globals = Signature("48 8D 15 ? ? ? ? 4C 8B C0 E8 ? ? ? ? 48 85 FF 48 89 1D").Scan().Add(3).Rip().As<std::int64_t**>();
		assert_ptr("ScriptGlobals", m_script_globals);

		{
			MemoryHandle temp = Signature("40 53 48 83 ec 20 e8 ? ? ? ? 48 8b d8 48 85 c0 74 12").Scan();
			m_model_spawn_bypass = temp.Add(0x1E).As<PVOID>();
			assert_ptr("ModelSpawnBypass", m_model_spawn_bypass);
			
		}

		m_convert_thread_to_fiber = GetProcAddress(GetModuleHandleA("kernel32.dll"), "ConvertThreadToFiber");
		assert_ptr("ConvertThreadToFiber", m_convert_thread_to_fiber);

		m_BlipList = Signature("41 83 e2 1f 49 c1 e8 05 41 8a ca 41 d3 e1 41 f7 d1").Scan().Sub(11).Add(3).Rip().As<rage::BlipList*>();
		assert_ptr("BlipList", m_BlipList);

		m_scriptEventHandler = Signature("40 53 48 81 EC F0 01 00 00").Scan().As<PVOID>();
		assert_ptr("CScriptedGameEvent::Handler", m_scriptEventHandler);

		__int64 TickCountEnd = GetTickCount64();
		__int64 TimeElapsed = TickCountEnd - TickCountStart;
		
		//g_Logger->Info("Took -> %i", TimeElapsed);

		g_native_invoker = std::make_unique<native_invoker>();
		g_native_invoker->cache_handlers();

		g_script_mgr = std::make_unique<script_mgr>();
	}

	GtaThread* base::Utils::FindScript(rage::joaat_t m_ScriptHash)
	{
		for (auto m_script : *g_Unity->m_script_threads)
		{
			if (m_script->m_script_hash == m_ScriptHash)
			{

				return m_script;
			}
		}

		return nullptr;
	}

	void base::Utils::NativeDump()
	{

		auto m_DumpPath = g_CheatPath; m_DumpPath /= "NativeDumpg-"; m_DumpPath += std::to_string(rand() % 0xFFFF).c_str(); m_DumpPath += ".dump";

		g_native_invoker->DumpNatives(m_DumpPath);

		g_Logger->Info("Dumped -> to -> %s", m_DumpPath.string().c_str());

	}

	char* base::Utils::GetPlayerFlag(CNetGamePlayer* pPlayer)
	{
		char flag[4] = {};

		if (pPlayer->GetPlayerPed() != 0)
		{
			if (pPlayer->GetPlayerPed()->HasGodModeFlag())
			{
				strcat(flag, "G");
			}

			if (pPlayer->GetWantedLevel() != 0)
			{
				strcat(flag, "W");
			}
		}

		return flag;
	}
	
}