#include "../../common.hpp"

namespace base::gui
{

    struct NetworkGuiConfig
    {
        std::int8_t m_Selected = 0;
    };

    extern NetworkGuiConfig g_NetworkGuiConfig;

    class NetworkGui
    {
    public:
        static void OnRender();

    };

}
