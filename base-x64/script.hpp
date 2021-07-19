#pragma once
#include "common.hpp"

namespace base
{
	class script
	{
	public:
		using func_t = void(*)();
	public:
		explicit script(func_t func, std::optional<std::size_t> stack_size = std::nullopt);
		~script();

		void tick();
		void yield(std::optional<std::chrono::high_resolution_clock::duration> time = std::nullopt);
		static script* get_current();
		void fiber_func();
		static void script_exception_handler(PEXCEPTION_POINTERS exp);
	private:
		void* m_script_fiber;
		void* m_main_fiber;
		func_t m_func;
		std::optional<std::chrono::high_resolution_clock::time_point> m_wake_time;
	};


}
