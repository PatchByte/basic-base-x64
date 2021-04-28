#pragma once
#include "common.hpp"
namespace base
{
	class log
	{
	private:
	public:
		std::ofstream  m_Output;
		std::ofstream  m_Input;
	public:
		log() = default;
		~log() = default;

		void initialize();
		void shutdown();

		void Info(const char* format, ...);
		void Error(const char* format, ...);

		void Log(const char* type, const char* format, std::va_list args);
	};

	inline std::unique_ptr<log> g_Logger;
}
