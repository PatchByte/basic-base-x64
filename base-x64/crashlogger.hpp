#pragma once
#include "common.hpp"

namespace base::crashlogger
{

	struct ScriptEvent
	{
		std::uint32_t m_script_hash;
		std::uint64_t m_sender_hash;
	};

	struct NetEvent
	{
		std::int32_t m_net_id;
		std::uint64_t m_sender_hash;
	};

	class Crashlogger
	{
	public:
		void	initialize();
		void  uninitialize();

		static LONG exception_handler(_EXCEPTION_POINTERS* ExceptionInfo);

		void add_script_event(ScriptEvent m_Event);
		void add_net_event(NetEvent m_Event);
	public:
		std::list<ScriptEvent> m_LastScriptEvents;
		std::list<NetEvent> m_LastNetEvents;
	};

	inline std::unique_ptr<Crashlogger> g_CrashLogger;

}