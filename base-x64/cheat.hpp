#pragma once
#include "common.hpp"
#include "logger.hpp"

namespace base
{

	class Cheat
	{
	private:
	public:
		Cheat() = default;
		~Cheat() = default;

		static void initialize();

		void init();
		void shutdown();
		void tick();
		static void init_script();
	public:
	};

	inline std::unique_ptr<Cheat> g_Cheat;
}