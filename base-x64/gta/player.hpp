#pragma once
#include "../common.hpp"
#include "ped_factory.hpp"

namespace rage
{
	struct netAddress
	{
		std::uint8_t m_field4;
		std::uint8_t m_field3;
		std::uint8_t m_field2;
		std::uint8_t m_field1;
	};

	class netPlayerData
	{
	public:
		char pad_0000[72]; //0x0000
		uint32_t N00000058; //0x0048
		netAddress m_online_ip; //0x004C
		uint32_t N00000059; //0x0050
		netAddress m_local_ip; //0x0054
		uint16_t m_local_port; //0x0058
		char pad_005A[10]; //0x005A
		int32_t host_token; //0x0064
		char pad_0068[8]; //0x0068
		uint64_t m_rockstar_id; //0x0070
		char pad_0078[12]; //0x0078
		char m_name[20]; //0x0084
	}; //Size: 0x0078


	class netPlayer
	{
	public:
		virtual ~netPlayer() = 0;

		virtual void Reset() = 0;

		virtual void m_10() = 0;

		virtual const char* GetName() = 0;

		virtual void m_20() = 0;

		virtual bool IsHost() = 0;

		virtual netPlayerData* get_net_data() = 0;
	};


}


class CNetGamePlayer : public rage::netPlayer
{
public:
	std::int8_t GetPlayerId()
	{

		return *(int*)((DWORD64)this + 0xC0);

	}

	CPed* GetPlayerPed()
	{

		DWORD64 playerInfo = *(DWORD64*)((DWORD64)this + 0xA0);

		if (playerInfo == 0)
		{
			return 0;
		}

		DWORD64 outPed = *(DWORD64*)((DWORD64)playerInfo + 0x1E8);

		return (CPed*)outPed;
	}

	std::int8_t GetWantedLevel()
	{
		DWORD64 playerInfo = *(DWORD64*)((DWORD64)this + 0xA0);

		if (playerInfo != 0)
		{
			return *(std::int8_t*)(playerInfo + 0x888);
		}

		return 0;
	}

};