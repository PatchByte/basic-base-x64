#include "../Gui.hpp"
#include "HomeGui.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

#include "../../logger.hpp"
#include "../../script_mgr.hpp"
#include "../../fiber_pool.hpp"
#include "../../natives.hpp"

#include "../../unity.hpp"

namespace base::gui
{

    HomeGuiConfig g_HomeGuiConfig = {};
    

    void base::gui::LSCConfig::create(int m_veh)
    {
        g_HomeGuiConfig.g_LSCConfig.clear();
        g_HomeGuiConfig.g_LSCConfig.m_selected_opt = 0;
        for (int i = 0; i <= 50; i++)
        {

            const char* modName = VEHICLE::GET_MOD_SLOT_NAME(m_veh, i);
            if (modName != 0)
            {
                int _len = strlen(modName);
                if (_len != 0)
                {
                   // g_Logger->Info("%s", modName);

                    LSCOption opt { };

                    opt.copy_name(modName);
                    

                    int idx = VEHICLE::GET_NUM_VEHICLE_MODS(m_veh, i) - 1;
                    
                    opt.m_selected = VEHICLE::GET_VEHICLE_MOD(m_veh, i);

                    for (int x = 0; x <= idx; x++)
                    {
                        script::get_current()->yield();
                        LSCOption::LSCEntry  entr{ };

                        const char* label = VEHICLE::GET_MOD_TEXT_LABEL(m_veh, i, x);

                        if (label != 0)
                        {
                            int _xen = strlen(label);

                            if (_xen != 0)
                            {
                                const char* text = HUD::_GET_LABEL_TEXT(label);
                                entr.copy_name(text);
                                entr.m_id = x;

                                opt.m_val.emplace(x, entr);
                            }
                        }
                    }

                    g_HomeGuiConfig.g_LSCConfig.m_options.emplace(i,std::move(opt));
                }
                
            }

            script::get_current()->yield();
        }
    }

    void HomeGui::OnRender()
    {
        ImGui::BeginTabBar("##home_tab_bar");

        

        if (ImGui::BeginTabItem("Self"))
        {
            ImGui::Checkbox("GodMode", &g_HomeGuiConfig.m_GodMode);
            ImGui::SameLine();
            ImGui::Checkbox("NeverWanted", &g_HomeGuiConfig.m_NeverWanted);
            ImGui::Separator();
            ImGui::SliderInt("##add_ammo_slider", &g_HomeGuiConfig.m_AddAmmo, 1, 1000);
            ImGui::SameLine();
            if (ImGui::Button("Add Ammo"))
            {
                g_fiber_pool->queue_job([] 
                    {
                        Ped     pId = PLAYER::PLAYER_PED_ID();
                        Hash    wid = 0;

                        if (WEAPON::GET_CURRENT_PED_WEAPON(pId, &wid, 1))
                        {
                            WEAPON::ADD_AMMO_TO_PED(pId, wid, g_HomeGuiConfig.m_AddAmmo);
                        }
                    });
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Vehicle"))
        {
            {
                static char buf[100] = {};

                ImGui::InputText("##vehicle_input", buf, 99);
                ImGui::SameLine();
                if (ImGui::Button("Spawn"))
                {
                    g_fiber_pool->queue_job([&]
                        {
                            std::uint32_t   m_hash = rage::joaat(buf);
                            Vector3         m_Coord = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), 1);

                            if(!(STREAMING::IS_MODEL_VALID(m_hash) && STREAMING::IS_MODEL_IN_CDIMAGE(m_hash)))
                            {
                                g_Logger->Info("%s -> not valid vehicle!", buf);
                                return;
                            }

                            while (!STREAMING::HAS_MODEL_LOADED(m_hash))
                            {
                                STREAMING::REQUEST_MODEL(m_hash);
                                script::get_current()->yield();
                            }

                            auto m_vehicle = VEHICLE::CREATE_VEHICLE(m_hash, m_Coord.x, m_Coord.y, m_Coord.z, 0, 1, 1, 0);

                            script::get_current()->yield();
                            STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(m_hash);
                            if (NETWORK::NETWORK_IS_SESSION_STARTED())
                            {
                                DECORATOR::DECOR_SET_INT(m_vehicle, "MPBitset", 0);
                                ENTITY::_SET_ENTITY_CLEANUP_BY_ENGINE(m_vehicle, TRUE); //True means it can be deleted by the engine when switching lobbies/missions/etc, false means the script is expected to clean it up.
                                auto networkId = NETWORK::VEH_TO_NET(m_vehicle);
                                if (NETWORK::NETWORK_GET_ENTITY_IS_NETWORKED(m_vehicle))
                                    NETWORK::SET_NETWORK_ID_EXISTS_ON_ALL_MACHINES(networkId, true);
                                VEHICLE::SET_VEHICLE_IS_STOLEN(m_vehicle, FALSE);
                            }

                            PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), m_vehicle, -1);
                        });
                }
            }
            ImGui::Separator();
            {
                static const char* nPlateText[] = { "Blue/White", "Yellow/black", "Yellow/Blue", "Blue/White2", "Blue/White3", "Yankton" };
                static int nPlateIndex = 0;

                ImGui::Combo("##numberplateindex", &nPlateIndex, nPlateText, 6);
                ImGui::SameLine();
                if (ImGui::Button("Set NumberPlate Color"))
                {


                    g_fiber_pool->queue_job([&]
                        {
                            auto pId = PLAYER::PLAYER_PED_ID();
                            auto vId = PED::GET_VEHICLE_PED_IS_IN(pId, 1);

                            VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT_INDEX(vId, nPlateIndex);
                        });

                }
            }

            {
                static char plateBuf[9] = {};

                ImGui::InputText("##number_plate_text", plateBuf, 8);
                
                ImGui::SameLine();
                if (ImGui::Button("Set NumberPlate Text"))
                {


                    g_fiber_pool->queue_job([&]
                        {
                            auto pId = PLAYER::PLAYER_PED_ID();
                            auto vId = PED::GET_VEHICLE_PED_IS_IN(pId, 1);

                            VEHICLE::SET_VEHICLE_NUMBER_PLATE_TEXT(vId, plateBuf);
                        });

                }
            }

            if (ImGui::Button("Max Speed"))
            {
                g_fiber_pool->queue_job([&]
                    {
                        auto pId = PLAYER::PLAYER_PED_ID();
                        auto vId = PED::GET_VEHICLE_PED_IS_IN(pId, 1);

                        VEHICLE::_SET_VEHICLE_MAX_SPEED(vId, FLT_MAX);
                        VEHICLE::SET_VEHICLE_CHEAT_POWER_INCREASE(vId, FLT_MAX);
                    });
            }
            ImGui::SameLine();
            if (ImGui::Button("Repair"))
            {
                g_fiber_pool->queue_job([] 
                    {
                        Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
                        VEHICLE::SET_VEHICLE_FIXED(vehicle);
                        VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(vehicle);
                        VEHICLE::SET_VEHICLE_DIRT_LEVEL(vehicle, 0);
                    });
            }
            ImGui::SameLine();
            if (ImGui::Button("Max"))
            {
                g_fiber_pool->queue_job([] 
                    {
                        Vehicle vehicle = PED::GET_VEHICLE_PED_IS_USING(PLAYER::PLAYER_PED_ID());
                        VEHICLE::SET_VEHICLE_MOD_KIT(vehicle, 0);
                        for (int i = 0; i < 50; i++)
                        {
                            


                            VEHICLE::SET_VEHICLE_MOD(vehicle, i, VEHICLE::GET_NUM_VEHICLE_MODS(vehicle, i) - 1, false);
                        }

                    });
            }

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("World"))
        {
            ImGui::Checkbox("Kill Peds", &g_HomeGuiConfig.m_KillPeds);
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }


    void base::gui::HomeGui::OnTick()
    {
        if (g_HomeGuiConfig.m_KillPeds == true)
        {
           
        
            struct nearbyEnts {
                int size;//still 32 bit integer
                int64_t entities[100];
            };

            nearbyEnts arr;
            arr.size = 100;
            int ped = PLAYER::PLAYER_PED_ID();
            int size = PED::GET_PED_NEARBY_PEDS(ped, (int*)&arr, ped);
            for (int i = 0; i < size; i++) {

                Ped cPe = arr.entities[i];


                if (!ENTITY::DOES_ENTITY_EXIST(cPe))
                {
                    continue;
                }

                if (PED::IS_PED_IN_ANY_VEHICLE(cPe, 1))
                {
                    TASK::CLEAR_PED_TASKS_IMMEDIATELY(cPe);
                }

                Vector3         m_Coord = PED::GET_PED_BONE_COORDS(cPe, PedBones::SKEL_Head, 0, 0, 0);

                MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(m_Coord.x, m_Coord.y + 3.f, m_Coord.z, m_Coord.x, m_Coord.y, m_Coord.z, 200, 1, rage::joaat("VEHICLE_WEAPON_PLAYER_BULLET"), ped, 1, 0, 200.0);

                script::get_current()->yield();
            }

            
        }
    }
}
