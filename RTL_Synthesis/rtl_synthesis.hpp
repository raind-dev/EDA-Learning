#include <iostream>
#include "netlist_parser.hpp"

namespace rtl_synthesis {
	class RTL_Synthesis {
	public:
		void execute();
		static RTL_Synthesis& get_instance() {
			if (m_instance == nullptr)
				m_instance = new RTL_Synthesis();
			return *m_instance;
		}
	private:
		RTL_Synthesis() = default;
		inline static RTL_Synthesis* m_instance = nullptr;
		netlist_parser::Netlist_Parser m_netlist_parser;
	};

}