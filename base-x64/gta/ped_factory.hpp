#pragma once
#include "fwddec.hpp"

#pragma pack(push, 1)
class CPed
{
public:
	char pad_0000[4296]; //0x0000
	void* m_playerInfo; //0x10C8

public:
	void SetGodModeFlag(bool flag)
	{
		DWORD64 pedOff = (DWORD64)(this);

		using uint = std::uint32_t;

		*(uint*)(pedOff + 0x188) = *(uint*)(pedOff + 0x188) & 0xfffffeff;
		*(uint*)(pedOff + 0x188) = *(uint*)(pedOff + 0x188) | (flag & 1) << 8;
	}

	bool HasGodModeFlag()
	{
		DWORD64 pedOff = (DWORD64)(this);

		using uint = std::uint32_t;

		return ((*(uint*)(pedOff + 0x188) >> 8) & 1);
	}

	void SetPlayerWantedLevel(std::int8_t lvl)
	{
		DWORD64 playerInfo = *(DWORD64*)((DWORD64)this + 0x10c8);

		if (playerInfo != 0)
		{
			*(std::int8_t*)(playerInfo + 0x888) = lvl;
		}
	}

	std::int8_t GetWantedLevel()
	{
		DWORD64 playerInfo = *(DWORD64*)((DWORD64)this + 0x10c8);

		if (playerInfo != 0)
		{
			return *(std::int8_t*)(playerInfo + 0x888);
		}

		return 0;
	}

	void SetWeaponInfClip(bool flag)
	{
		DWORD64 pedOff = (DWORD64)(this);

		using uint = std::uint32_t;

		DWORD64 playerInfo = *(DWORD64*)((DWORD64)this + 0x10c8);


	}
};
#pragma pack(pop)

class CPedFactory
{
public:
	virtual ~CPedFactory() = default;

	CPed *m_local_ped;
};
