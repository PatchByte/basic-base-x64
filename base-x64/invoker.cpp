#include "common.hpp"
#include "crossmap.hpp"
#include "invoker.hpp"
#include "logger.hpp"
#include "unity.hpp"
#include "logger.hpp"

namespace base
{
	native_call_context::native_call_context()
	{
		m_return_value = &m_return_stack[0];
		m_args = &m_arg_stack[0];
	}

	void native_invoker::cache_handlers()
	{
		for (const rage::scrNativeMapping& mapping : crossmap::g_Crossmap)
		{
			rage::scrNativeHandler handler = g_Unity->m_get_native_handler(
				g_Unity->m_native_registration_table, mapping.second);

			if (handler != 0)
			{
				m_handler_cache.emplace(mapping.first, handler);
				continue;
			}

			g_Logger->Error("Native Handler -> %llx <=> %llx <- not found!", mapping.first, mapping.second);
		}

		g_Logger->Info("Cached %i handlers!", m_handler_cache.size());
	}

	void native_invoker::begin_call()
	{
		m_call_context.reset();
	}

	void native_invoker::end_call(rage::scrNativeHash hash)
	{
		if (rage::scrNativeHandler it = this->m_handler_cache[hash])
		{
			rage::scrNativeHandler handler = it;

			handler(&m_call_context);
			g_Unity->m_fix_vectors(&m_call_context);
		}
	}

	void base::native_invoker::DumpNatives(std::filesystem::path fileOut)
	{
		std::ofstream m_Output;

		m_Output.open(fileOut, std::ios::trunc);
		DWORD64 moduleBase = (DWORD64) GetModuleHandleA(0);

		for (auto m_entry : crossmap::g_Crossmap)
		{
			DWORD64 calc = ((DWORD64) this->m_handler_cache[m_entry.first]) - moduleBase;
			char buf[100] = {};

			sprintf_s(buf, "%llx <=> %llx -> %llx\n", m_entry.first, m_entry.second, calc);

			m_Output << buf;
		}

		m_Output.flush();
		m_Output.close();
	}

	void base::native_invoker::set_model_check(bool tog)
	{
		__try
		{
			if (tog == true)
			{
				*(unsigned short*)g_Unity->m_model_spawn_bypass = 0x9090;
			}
			else
			{
				*(unsigned short*)g_Unity->m_model_spawn_bypass = 0x0574;
			}
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			g_Logger->Info("Model Check!");
		}
	}
}