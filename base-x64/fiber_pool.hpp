#pragma once
#include "common.hpp"

namespace base
{
	class fiber_pool
	{
	public:
		explicit fiber_pool(std::size_t num_fibers);

		void queue_job(std::function<void()> func);
		void queue_thread(std::function<void()> func);

		void fiber_tick();
		void thread_tick();
		static void fiber_func();
	private:
		std::recursive_mutex m_mutex;
		std::recursive_mutex m_mutex2;
		std::stack<std::function<void()>> m_jobs;
		std::stack<std::function<void()>> m_threadjobs;
	};

	inline std::unique_ptr<fiber_pool> g_fiber_pool;
}
