#include "../Gui.hpp"
#include "NetworkGui.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

#include "../../logger.hpp"
#include "../../unity.hpp"

namespace base::gui
{

    NetworkGuiConfig g_NetworkGuiConfig = {};

    void NetworkGui::OnRender()
    {
        Utils::RefreshPlayerList();

        ImGui::BeginTabBar("##network_tab");

        if (ImGui::BeginTabItem("Players"))
        {

            ImGui::BeginChild("Ps", ImVec2(200, -1), true);

            {
                for (auto it = g_Players.begin(); it != g_Players.end(); it++)
                {
                    const char* name = it->second->GetName();

                    if (name != 0)
                    {
                        char buf[32 * 2] = {};

                        

                        strcat(buf, name);
                        strcat(buf, " ");
                        strcat(buf, Utils::GetPlayerFlag(it->second));

                        if (ImGui::Selectable(buf, it->first == g_NetworkGuiConfig.m_Selected))
                        {
                            g_NetworkGuiConfig.m_Selected = it->first;
                        }
                    }
                }
            }

            ImGui::EndChild();

            ImGui::SameLine();

            ImGui::BeginChild("sP", ImVec2(-1, -1), true);

            {
                CNetGamePlayer* pPlayer = g_Players[g_NetworkGuiConfig.m_Selected];
               

                if (pPlayer != 0 && pPlayer->get_net_data() != 0)
                {
                    auto m_ip = pPlayer->get_net_data()->m_online_ip;
                    /*if (ImGui::BeginTable("##table1", 3))
                    {
                        for (int row = 0; row < 4; row++)
                        {
                            ImGui::TableNextRow();
                            for (int column = 0; column < 3; column++)
                            {
                                ImGui::TableSetColumnIndex(column);
                                ImGui::Text("Row %d Column %d", row, column);
                            }
                        }
                        ImGui::EndTable();
                    }*/

                    if (ImGui::BeginTable("##Player_infos", 4))
                    {
                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Name");
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("Ip");
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("Rid");
                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text("Flags");
                        //

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text(pPlayer->GetName());
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("%i.%i.%i.%i", m_ip.m_field1, m_ip.m_field2, m_ip.m_field3, m_ip.m_field4);
                        ImGui::TableSetColumnIndex(2);
                        ImGui::Text("%u", pPlayer->get_net_data()->m_rockstar_id);
                        ImGui::TableSetColumnIndex(3);
                        ImGui::Text(Utils::GetPlayerFlag(pPlayer));

                        ImGui::EndTable();
                    }

                }
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

}
