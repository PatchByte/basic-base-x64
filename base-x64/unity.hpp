#pragma once
#include "common.hpp"
#include "gta/fwddec.hpp"
#include "gta/enums.hpp"
#include "gta/natives.hpp"
#include "gta/joaat.hpp"
#include "gta/array.hpp"
#include "gta/blip.hpp"
#include "gta/player.hpp"
#include "natives.hpp"
namespace base
{
	

	class Unity
	{
	private:
		using get_native_handler_t = rage::scrNativeHandler(*)(rage::scrNativeRegistrationTable*, rage::scrNativeHash);
		using fix_vectors_t = void(*)(rage::scrNativeCallContext*);
		using get_player_by_id = CNetGamePlayer * (*)(std::int8_t);
	public:
		
		HWND m_GameWindow = 0;

		eGameState* m_game_state{};
		bool* m_is_session_started{};

		CPedFactory** m_ped_factory{};

		rage::scrNativeRegistrationTable* m_native_registration_table{};
		get_native_handler_t m_get_native_handler{};
		fix_vectors_t m_fix_vectors{};

		rage::atArray<GtaThread*>* m_script_threads{};
		rage::scrProgramTable* m_script_program_table{};
		void* m_run_script_threads{};
		std::int64_t** m_script_globals{};

		rage::BlipList* m_BlipList;

		get_player_by_id m_get_player_by_id;

		PVOID m_model_spawn_bypass;

		PVOID m_convert_thread_to_fiber;

		PVOID m_scriptEventHandler{};
	public:
		void init();
	public:
		
	};

	inline std::unique_ptr<Unity> g_Unity;
	inline std::map<std::int8_t, CNetGamePlayer*> g_Players;

	class Utils
	{
	public:
		static void NativeDump();

		static GtaThread* FindScript(rage::joaat_t m_ScriptHash);
		static char* GetPlayerFlag(CNetGamePlayer* pPlayer);

		template <typename F, typename ...Args>
		static inline void ExecuteAsScript(rage::joaat_t m_ScriptHash, F&& callback, Args &&...args)
		{
			auto tls_ctx = rage::tlsContext::get();
			for (auto thread : *g_Unity->m_script_threads)
			{
				if (!thread || !thread->m_context.m_thread_id || thread->m_context.m_script_hash != m_ScriptHash)
					continue;

				auto og_thread = tls_ctx->m_script_thread;

				tls_ctx->m_script_thread = thread;
				tls_ctx->m_is_script_thread_active = true;

				std::invoke(std::forward<F>(callback), std::forward<Args>(args)...);

				tls_ctx->m_script_thread = og_thread;
				tls_ctx->m_is_script_thread_active = og_thread != nullptr;

				return;
			}
		}

		struct nearbyRes
		{
			struct nearbyEnts {
				int size;//still 32 bit integer
				int64_t entities[100];
			};

			int realSize = 0;
			nearbyEnts realResult;
		};

		static inline nearbyRes GetNearbyPeds(Ped myPed)
		{
			
			nearbyRes res;
			res.realResult.size = 100;
			int ped = myPed;
			int size = PED::GET_PED_NEARBY_PEDS(ped, (int*)&res.realResult, ped);
			
			res.realSize = size;

			return res;
			
		}

		static inline void RefreshPlayerList()
		{
			g_Players.clear();
			for (std::int8_t i = -1; i <= 33; i++)
			{
				auto pPlayer = g_Unity->m_get_player_by_id(i);

				if (pPlayer != 0)
				{
					g_Players[i] = pPlayer;
				}
			}
		}

	};
}