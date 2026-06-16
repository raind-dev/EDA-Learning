#include <unordered_map>
#include <typeindex>
#include <memory>
#include <cassert>
#include <print>

namespace registry {

	using entity_id = uint32_t;

	struct IComponentPool {
		virtual ~IComponentPool() = default;
	};

	template<typename T>
	struct component_pool : IComponentPool {
		std::unordered_map<entity_id, T> components;
	};

    class registry {
    public:
        registry() = default;
        ~registry() = default;

        registry(const registry&) = delete; // delete copy constructor
        registry& operator=(const registry&) = delete; // delete copy from assignment operator

        registry(registry&&) = default; // default move constructor (&& means rvalue reference)
        registry& operator=(registry&&) = default;

        entity_id create_entity() {
            return current_id++;
        }

        template<typename T>
        void add_component(entity_id id, const T& component) {
            auto& pool = get_or_create_pool<T>();
            pool.components[id] = component;
        }

        template<typename T>
        bool has_component(entity_id id) {
            auto& pool = get_or_create_pool<T>();
            return pool.components.find(id) != pool.components.end();
        }

        template<typename T>
        T& get_component(entity_id id) {
            auto& pool = get_or_create_pool<T>();
            auto it = pool.components.find(id);
            assert(it != pool.components.end() && "Component does not exist for this entity");
            return it->second;
        }

        template<typename T>
        void remove_component(entity_id id) {
            auto& pool = get_or_create_pool<T>();
            pool.components.erase(id);
        }

        template<typename T>
        std::vector<entity_id> view() {
            std::vector<entity_id> entities;
            auto& pool = get_or_create_pool<T>();
            for (auto& [id, comp] : pool.components) {
                entities.push_back(id);
            }
            return entities;
        }

    private:
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools;
        entity_id current_id = 0;

        template<typename T>
        component_pool<T>& get_or_create_pool() {
            std::type_index typeIdx(typeid(T));
            auto it = component_pools.find(typeIdx);
            if (it == component_pools.end()) {
                auto ptr = std::make_unique<component_pool<T>>();
                component_pool<T>* raw_ptr = ptr.get();
                component_pools[typeIdx] = std::move(ptr);
                return *raw_ptr;
            }
            return *static_cast<component_pool<T>*>(it->second.get());
        }
    };
}