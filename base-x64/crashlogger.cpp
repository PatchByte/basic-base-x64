#include "crashlogger.hpp"
#include "logger.hpp"

#include <Windows.h>


namespace base::crashlogger
{
	static PVOID g_OldExcpetionHandler = 0;

	void base::crashlogger::Crashlogger::initialize()
	{
		g_OldExcpetionHandler = SetUnhandledExceptionFilter(base::crashlogger::Crashlogger::exception_handler);
	}

	void base::crashlogger::Crashlogger::uninitialize()
	{
		SetUnhandledExceptionFilter(static_cast<LPTOP_LEVEL_EXCEPTION_FILTER>(g_OldExcpetionHandler));
	}

	LONG base::crashlogger::Crashlogger::exception_handler(_EXCEPTION_POINTERS* ExceptionInfo)
	{
		g_Logger->Error("-------- EXCPEPTION");
		g_Logger->Error("***mayday****");
		g_Logger->Error("REGISTERS -> ");
		char regbuf[1024] = {};

		auto ctx = ExceptionInfo->ContextRecord;
		DWORD64 RAX = ctx->Rax, RBX = ctx->Rbx, RCX = ctx->Rcx, RDX = ctx->Rdx, Rdi = ctx->Rdi, Rip = ctx->Rip, Rsp = ctx->Rsp;

		sprintf_s(regbuf, "RAX %p; RBX %p; RCX %p; RDX %p; Rdi %p; Rip %p; Rsp %p", RAX, RBX, RCX, RDX, Rdi, Rip, Rsp);

		g_Logger->Error(regbuf);

		g_Logger->Error("-------- NetEvents");

		for (auto m_it = g_CrashLogger->m_LastNetEvents.begin(); m_it != g_CrashLogger->m_LastNetEvents.end(); m_it++)
		{
			g_Logger->Error("Sender -> %llx; NetEvent -> %i", m_it->m_sender_hash, m_it->m_net_id);
		}

		g_Logger->Error("-------- ScriptEvents");

		for (auto m_it = g_CrashLogger->m_LastScriptEvents.begin(); m_it != g_CrashLogger->m_LastScriptEvents.end(); m_it++)
		{
			g_Logger->Error("Sender -> %llx; ScriptHash -> %u", m_it->m_sender_hash, m_it->m_script_hash);
		}

		g_Logger->Error("-------- END");

		SuspendThread(GetCurrentThread());
		while (true);


		return EXCEPTION_EXECUTE_HANDLER;
	}

	void base::crashlogger::Crashlogger::add_script_event(ScriptEvent m_Event)
	{
		if(g_CrashLogger->m_LastScriptEvents.size() == 200)
		{
			g_CrashLogger->m_LastScriptEvents.pop_back();
		}

		g_CrashLogger->m_LastScriptEvents.push_front(m_Event);
	}

	void base::crashlogger::Crashlogger::add_net_event(NetEvent m_Event)
	{
		if (g_CrashLogger->m_LastNetEvents.size() == 50)
		{
			g_CrashLogger->m_LastNetEvents.pop_back();
		}

		g_CrashLogger->m_LastNetEvents.push_front(m_Event);
	}
}