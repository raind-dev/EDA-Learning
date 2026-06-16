#include <iostream>
#include "rtl_synthesis.hpp"

int main()
{
    rtl_synthesis::RTL_Synthesis& rs = rtl_synthesis::RTL_Synthesis::get_instance();
    rs.execute();
    return 0;
}