#include "../../common.hpp"
#include "../../script.hpp"

namespace base::gui
{

    struct LSCConfig
    {
        struct LSCOption
        {
            struct LSCEntry
            {
                void copy_name(const char* _name)
                {
                    size_t _name_len = strlen(_name);

                    this->m_name = (char*)malloc(_name_len + 1);
                    strcpy(m_name, _name);
                }

                void clear()
                {
                    m_id = 0;
                    m_name = (char*)"unknown";
                }

                void destroy()
                {
                    if (m_name != 0)
                        free(m_name);

                    clear();
                }

                int m_id;
                char* m_name;
            };

            void clear()
            {
                m_key = (char*)"unknwon";
                for (auto it = m_val.begin(); it != m_val.end(); it++)
                {
                    it->second.destroy();
                }

                m_val.clear();
            }

            void destroy()
            {
                if (m_key != 0)
                    free(m_key);

                clear();
            }

            void copy_name(const char* _name)
            {
                size_t _name_len = strlen(_name);

                this->m_key = (char*)malloc(_name_len + 1);
                //memcpy(m_key, _name, _name_len);

                strcpy(m_key, _name);
            }

            char* m_key;
            std::map<int, LSCEntry> m_val;
            int m_selected;

        };

        void clear()
        {
            for (auto it = m_options.begin(); it != m_options.end(); it++)
            {
                it->second.destroy();

                script::get_current()->yield();
            }
            m_options.clear();
        }

        void create(int m_veh);

        std::map<int, LSCOption> m_options;
        int m_selected_opt = 0;
    };

    struct HomeGuiConfig
    {
        bool m_GodMode = false;
        bool m_NeverWanted = false;
        LSCConfig g_LSCConfig = {};
        int m_AddAmmo = 5;

        bool m_KillPeds = false;
    };

    

    extern HomeGuiConfig g_HomeGuiConfig;

    class HomeGui
    {
    public:
        static void OnRender();
        static void OnTick();
    };

}
