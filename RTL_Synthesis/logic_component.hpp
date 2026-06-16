#include <vector>

namespace logic_component {

	enum class pin_type : int {
		UNKNOWN,
		INPUT,
		OUTPUT,
		INOUTPUT
	};
	enum class gate_type : int {
		UNKNOWN,
		AND,
		OR
	};
	struct Pin_Tag {};
	struct Gate_Tag {};
	struct Pin {
		std::string name;
		pin_type type = pin_type::UNKNOWN;
	};
	struct Gate {
		std::vector<Pin> input_pins;
		std::vector<Pin> output_pins;
		gate_type type = gate_type::UNKNOWN;
	};
}