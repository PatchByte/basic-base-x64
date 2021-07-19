#pragma once
#include "common.hpp"
#include "script.hpp"

namespace base
{
	class script_mgr
	{
	public:

		void add_script(std::unique_ptr<script> script);
		void remove_all_scripts();

		void tick();
	private:
		void tick_internal();
	private:
		std::recursive_mutex m_mutex;
		std::vector<std::unique_ptr<script>> m_scripts;
	};

	inline std::unique_ptr<script_mgr> g_script_mgr;
}
