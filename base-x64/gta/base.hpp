#pragma once

namespace rage
{
	class CViewPort
	{
	public:
		char _0x0000[0x24C];
		float fViewMatrix[0x10];
	};

	class datBase
	{
	public:
		virtual ~datBase() = default;
	};

	class pgBase
	{
	public:
		virtual ~pgBase() = default;
	private:
		void *m_pgunk;
	};
}
