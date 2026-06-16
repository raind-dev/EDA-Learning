#include <iostream>
#include <sstream>
#include <unordered_set>
#include <set>
#include <print>
#include "logic_component_creator.hpp"

namespace netlist_parser {
	enum class parsing_stage : int {
		NONE = 0,
		PARSE_DECLARATION,
		PARSE_BODY
	};

	class Syntax_Parser;

	class Syntax {
	public:
		Syntax(std::shared_ptr<Syntax_Parser>& sp) : m_syntax_parser{ sp } {}
		virtual ~Syntax() = default;
		virtual bool parse_syntax(const std::vector<std::string>& words) = 0;
		virtual bool check_syntax(const std::vector<std::string>& words, size_t& index) = 0;
		virtual bool is_the_end(const std::vector<std::string>& words) = 0;
	protected:
		std::shared_ptr<Syntax_Parser>& m_syntax_parser;
	};

	class Syntax_Parser {
	public:
		Syntax_Parser(std::unique_ptr<Syntax> s) : m_syntax{ std::move(s) }, m_registry{ std::make_unique<registry::registry>() }, m_stage{ parsing_stage::NONE } {}
		Syntax_Parser() : m_syntax{ nullptr }, m_registry{ std::make_unique<registry::registry>()}, m_stage{parsing_stage::NONE} {}
		~Syntax_Parser() = default;
		bool parse(const std::vector<std::string>& words) {
			return m_syntax->parse_syntax(words);
		}
		void set_syntax(std::unique_ptr<Syntax> s) {
			m_syntax = std::move(s);
		}
		parsing_stage get_stage() const noexcept {
			return m_stage;
		}
		void set_stage(parsing_stage s) {
			m_stage = s;
		}
		registry::registry* get_registry() noexcept {
			return !m_registry ? nullptr : m_registry.get();
		}
		void push_component(registry::entity_id id) noexcept {
			m_logic_components.push_back(id);
		}
		void dump_logic_components_info() {
			std::println();
			for (const registry::entity_id id : m_logic_components) {
				if (m_registry->has_component<logic_component::Pin_Tag>(id)) {
					std::println("Pin name: {}", m_registry->get_component<logic_component::Pin>(id).name);
					std::println("Pin type: {}", int(m_registry->get_component<logic_component::Pin>(id).type));
				}
				else if (m_registry->has_component<logic_component::Gate>(id)) {
					logic_component::Gate gate = m_registry->get_component<logic_component::Gate>(id);
					if (gate.type == logic_component::gate_type::AND)
						std::println("AND Gate:");
					else if (gate.type == logic_component::gate_type::OR)
						std::println("OR Gate:");
					for (logic_component::Pin p : gate.input_pins) {
						std::println("Input pin: {}", p.name);
					}
					for (logic_component::Pin p : gate.output_pins) {
						std::println("Output pin: {}", p.name);
					}
				}
			}
		}
		void draw_symbol();
	private:
		std::unique_ptr<Syntax> m_syntax;
		std::unique_ptr<registry::registry> m_registry;
		std::vector<registry::entity_id> m_logic_components;
		parsing_stage m_stage;
	};

	class Declaration_Syntax : public Syntax {
	public:
		Declaration_Syntax(std::shared_ptr<Syntax_Parser>& sp) : Syntax(sp) {}
		~Declaration_Syntax() = default;
		bool parse_syntax(const std::vector<std::string>& words) override;
		bool check_syntax(const std::vector<std::string>& words, size_t& index) override;
		bool is_the_end(const std::vector<std::string>& words) override;
	private:
		void parse_arguments(const std::vector<std::string>& words, size_t index);
	};

	class Body_Syntax : public Syntax {
	public:
		Body_Syntax(std::shared_ptr<Syntax_Parser>& sp) : Syntax(sp) {}
		~Body_Syntax() = default;
		bool parse_syntax(const std::vector<std::string>& words) override;
		bool check_syntax(const std::vector<std::string>& words, size_t& index) override;
		bool is_the_end(const std::vector<std::string>& words) override;
	private:
		void parse_gates(const std::vector<std::string>& words);
	};

	class Netlist_Parser {
	public:
		Netlist_Parser() : m_syntax_parser{ std::make_shared<Syntax_Parser>() } {}
		~Netlist_Parser() = default;
		void parse_text();
	private:
		bool execute_syntax_parser(const std::vector<std::string>& words);
	private:
		std::shared_ptr<Syntax_Parser> m_syntax_parser;
	};
}