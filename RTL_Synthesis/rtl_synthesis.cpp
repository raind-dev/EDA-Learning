#include "rtl_synthesis.hpp"

namespace rtl_synthesis {

    void RTL_Synthesis::execute() {
        m_netlist_parser.parse_text();
    }
}
