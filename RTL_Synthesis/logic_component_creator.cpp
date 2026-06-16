#include "logic_component_creator.hpp"
#include <print>

namespace logic_component_creator {

	registry::entity_id Logic_Component_Creator::create_pin(logic_component::pin_type t, const std::string& pin_name) {
		using namespace registry;
		entity_id pin_id = m_registry->create_entity();
		logic_component::Pin_Tag pin_tag;
		logic_component::Pin pin{pin_name, t};
		m_registry->add_component<logic_component::Pin_Tag>(pin_id, pin_tag);
		m_registry->add_component<logic_component::Pin>(pin_id, pin);
		return pin_id;
	}
	registry::entity_id Logic_Component_Creator::create_AND_gate(const std::string& input1, const std::string& input2, const std::string& output) {
		using namespace registry;
		logic_component::Pin input_pin1 = get_pin(input1);
		logic_component::Pin input_pin2 = get_pin(input2);
		logic_component::Pin output_pin = get_pin(output);
		entity_id gate_id = m_registry->create_entity();
		logic_component::Gate_Tag gate_tag;
		logic_component::Gate and_gate{ .input_pins = {input_pin1, input_pin2}, .output_pins = {output_pin}, .type = logic_component::gate_type::AND };
		m_registry->add_component<logic_component::Gate_Tag>(gate_id, gate_tag);
		m_registry->add_component<logic_component::Gate>(gate_id, and_gate);
		return gate_id;
	}
	registry::entity_id Logic_Component_Creator::create_OR_gate(const std::string& input1, const std::string& input2, const std::string& output) {
		using namespace registry;
		logic_component::Pin input_pin1 = get_pin(input1);
		logic_component::Pin input_pin2 = get_pin(input2);
		logic_component::Pin output_pin = get_pin(output);
		entity_id gate_id = m_registry->create_entity();
		logic_component::Gate_Tag gate_tag;
		logic_component::Gate or_gate{ .input_pins = {input_pin1, input_pin2}, .output_pins = {output_pin}, .type = logic_component::gate_type::OR };
		m_registry->add_component<logic_component::Gate_Tag>(gate_id, gate_tag);
		m_registry->add_component<logic_component::Gate>(gate_id, or_gate);
		return gate_id;
	}
	logic_component::Pin Logic_Component_Creator::get_pin(const std::string& pin_name) {
		using namespace registry;
		std::vector<entity_id> pins = m_registry->view<logic_component::Pin_Tag>();
		for (entity_id id : pins) {
			logic_component::Pin p = m_registry->get_component<logic_component::Pin>(id);
			if (p.name == pin_name)
				return p;
		}
		assert(true && "\033[33m[Warning] Error: can not find the pin: {}\033[0m", pin_name);
	}
}