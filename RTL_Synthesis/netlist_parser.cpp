#include "netlist_parser.hpp"

namespace netlist_parser {
    void Netlist_Parser::parse_text() {
        std::ios_base::sync_with_stdio(false);
        std::string line;
        while (std::getline(std::cin, line)) {
            std::stringstream ss(line);
            std::string word;
            std::vector<std::string> words;
            while (ss >> word) {
                words.push_back(word);
            }
            execute_syntax_parser(words);
            if (m_syntax_parser->get_stage() == parsing_stage::NONE)
                break;
        }
    }

    bool Netlist_Parser::execute_syntax_parser(const std::vector<std::string>& words) {
        if (m_syntax_parser->get_stage() == parsing_stage::NONE) {
            m_syntax_parser->set_syntax(std::make_unique<Declaration_Syntax>(m_syntax_parser));
        }
        return m_syntax_parser->parse(words);
    }

    void Syntax_Parser::draw_symbol() {
        std::vector<registry::entity_id> gates, input_pins, output_pins;
        int gate_number = 0;
        for (registry::entity_id id : m_logic_components) {
            if (m_registry->has_component<logic_component::Gate_Tag>(id)) {
                gates.push_back(id);
                gate_number++;
            }
            if (m_registry->has_component<logic_component::Pin_Tag>(id)) {
                logic_component::Pin p = m_registry->get_component<logic_component::Pin>(id);
                if (p.type == logic_component::pin_type::INPUT)
                    input_pins.push_back(id);
                else if (p.type == logic_component::pin_type::OUTPUT)
                    output_pins.push_back(id);
            }
        }
        std::println("gate number: {}, input pin number: {}, output pin number: {}", gates.size(), input_pins.size(), output_pins.size());
        std::println();
        std::println("  ---------------  ");
        for (int i = 0; i < gate_number; i++) {
            for (int j = 0; j < 3; j++) {
                if (j % 3 == 0 || j % 3 == 2) {
                    if (input_pins.size() != 0) {
                        registry::entity_id id = input_pins.front();
                        logic_component::Pin p = m_registry->get_component<logic_component::Pin>(id);
                        std::println("{}-|             |  ", p.name);
                        input_pins.erase(input_pins.begin());
                    }
                    else {
                        std::println("  |             |  ");
                    }
                }
                else if (j % 3 == 1) {
                    registry::entity_id p_id = output_pins.front();
                    registry::entity_id g_id = gates.front();
                    logic_component::Pin p = m_registry->get_component<logic_component::Pin>(p_id);
                    logic_component::Gate g = m_registry->get_component<logic_component::Gate>(g_id);
                    if (g.type == logic_component::gate_type::AND) {
                        std::println("  |     AND     |-{}", p.name);
                    }
                    else if (g.type == logic_component::gate_type::OR) {
                        std::println("  |      OR     |-{}", p.name);
                    }
                    output_pins.erase(output_pins.begin());
                    gates.erase(gates.begin());
                }
            }
            if (i+1 < gates.size())
                std::println();
        }
        std::println("  ---------------  ");
    }

    bool Declaration_Syntax::parse_syntax(const std::vector<std::string>& words) {
        size_t index = 0;
        if (!check_syntax(words, index))
            return false;
        parse_arguments(words, index);
        if (is_the_end(words)) {
            m_syntax_parser->set_stage(parsing_stage::PARSE_BODY);
            m_syntax_parser->set_syntax(std::make_unique<Body_Syntax>(m_syntax_parser));
        }
        return true;
    }

    void Declaration_Syntax::parse_arguments(const std::vector<std::string>& words, size_t index) {
        using namespace logic_component_creator;
        Logic_Component_Creator* lcc = new Logic_Component_Creator(m_syntax_parser->get_registry());
        for (size_t i = index; i < words.size(); i += 3) {
            if (words[i] == "input" && i+2 < words.size()) {
                registry::entity_id id = lcc->create_pin(logic_component::pin_type::INPUT, words[i + 2]);
                m_syntax_parser->push_component(id);
            }
            else if (words[i] == "output" && i + 2 < words.size()) {
                registry::entity_id id = lcc->create_pin(logic_component::pin_type::OUTPUT, words[i + 2]);
                m_syntax_parser->push_component(id);
            }
        }
    }

    bool Declaration_Syntax::check_syntax(const std::vector<std::string>& words, size_t& index) {
        if (m_syntax_parser->get_stage() == parsing_stage::NONE) {
            const std::string& word = words[2];
            if (words[0] != "module") {
                std::println(stderr, "Syntax Error: can not find the keyword \"module\" at the beging of the text.");
                return false;
            }
            else if (words.size() < 3) {
                std::println(stderr, "Syntax Error: the declaration syntax must be \"module {module_name} ( ... )\".");
                return false;
            }
            else if (word.find("(") == std::string::npos) {
                std::println(stderr, "Syntax Error: there's no \'\(\' after the module name.");
                std::println(stderr, "Syntax Error: the declaration syntax must be \"module {module_name} ( ... )\".");
                return false;
            }
            m_syntax_parser->set_stage(parsing_stage::PARSE_DECLARATION);
            if (words.size() == 3)
                return true;
            index = 3;
        }
        return true;
    }

    bool Declaration_Syntax::is_the_end(const std::vector<std::string>& words) {
        const std::string& word = words[words.size() - 1];
        if (word.find(");") != std::string::npos) 
            return true;
        if (word.find(")") != std::string::npos) 
            std::println(stderr, "Syntax Error: expected \';\' after \')\'.");
        return false;
    }

    bool Body_Syntax::parse_syntax(const std::vector<std::string>& words) {
        if (is_the_end(words)) {
            m_syntax_parser->set_stage(parsing_stage::NONE);
            m_syntax_parser->dump_logic_components_info(); // for debug
            m_syntax_parser->draw_symbol();
            return true;
        }
        size_t index = 0;
        if (!check_syntax(words, index))
            return false;
        parse_gates(words);
        if (is_the_end(words)) {
            m_syntax_parser->set_stage(parsing_stage::NONE);
            m_syntax_parser->dump_logic_components_info(); // for debug
            m_syntax_parser->draw_symbol();
        }
        return true;
    }

    void Body_Syntax::parse_gates(const std::vector<std::string>& words) {
        using namespace logic_component_creator;
        Logic_Component_Creator* lcc = new Logic_Component_Creator(m_syntax_parser->get_registry());
        if (words[4] == "|") {
            registry::entity_id id = lcc->create_OR_gate(words[3], words[5], words[1]);
            m_syntax_parser->push_component(id);
        }
        else if (words[4] == "&") {
            registry::entity_id id = lcc->create_AND_gate(words[3], words[5], words[1]);
            m_syntax_parser->push_component(id);
        }
    }

    bool Body_Syntax::check_syntax(const std::vector<std::string>& words, size_t& index) {
        if (words[index] != "assign") {
            std::println(stderr, "Syntax Error: can not find the keyword \"assign\" at the beging of the line.");
            return false;
        } 
        index = 2;
        if (words[index] != "=") {
            std::println(stderr, "Syntax Error: expected \'=\' for assignment operation.");
            return false;
        }
        index = words.size() - 1;
        const std::string& word = words[index];
        if (word.find(";") == std::string::npos) {
            std::println(stderr, "Syntax Error: expected \';\' after expression.");
            return false;
        }
        return true;
    }

    bool Body_Syntax::is_the_end(const std::vector<std::string>& words) {
        const std::string& word = words[words.size() - 1];
        if (word == "endmodule") 
            return true;
        return false;
    }
}