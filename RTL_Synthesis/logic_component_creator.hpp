#include "registry.hpp"
#include "logic_component.hpp"

namespace logic_component_creator {

	class Logic_Component_Creator {
	private:
		registry::registry* m_registry;
	public:
		Logic_Component_Creator() : m_registry{ nullptr } {}
		Logic_Component_Creator(registry::registry* r) : m_registry{ r } {}
		~Logic_Component_Creator() = default;
		void set_registry(registry::registry* r) { m_registry = r; }
		[[nodiscard]]
		registry::entity_id create_pin(logic_component::pin_type t, const std::string& pin_name);
		[[nodiscard]]
		registry::entity_id create_AND_gate(const std::string& input1, const std::string& input2, const std::string& output);
		[[nodiscard]]
		registry::entity_id create_OR_gate(const std::string& input1, const std::string& input2, const std::string& output);
	private:
		logic_component::Pin get_pin(const std::string& pin_name);
	};

}