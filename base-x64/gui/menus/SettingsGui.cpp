#include "../Gui.hpp"
#include "SettingsGui.hpp"
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_internal.h"

#include "../../logger.hpp"
#include "../../unity.hpp"

#include "../../gta/script_thread.hpp"

namespace base::gui
{

    SettingsGuiConfig g_SettingsGuiConfig = {};

    void SettingsGui::OnRender()
    {
        ImGui::BeginTabBar("##settings_tab");

        if (ImGui::BeginTabItem("Settings"))
        {
            ImGui::PushFont(g_Neon);
            ImGui::Text("This is a Example Text with z!");
            ImGui::PopFont();

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Utils"))
        {
            if (ImGui::Button("Dump Natives!"))
            {
                Utils::NativeDump();
            }

            if (ImGui::Button("Dump Scripts!"))
            {
                for (auto m_script : *g_Unity->m_script_threads)
                {
                    if (!m_script || !m_script->m_context.m_thread_id)
                        continue;

                  
                        g_Logger->Info("%s -> %u | %x", m_script->m_name, m_script->m_script_hash, m_script->m_script_hash);
                }
            }

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

}
