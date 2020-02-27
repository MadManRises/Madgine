#pragma once

#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DLL_IMPORT_VARIABLE2(const EntityComponentCollector::ComponentRegistrator<T>, _reg, typename T);

        DLL_IMPORT_VARIABLE2(const char *const, _componentName, typename T);

        template <typename T, typename Base>
        using EntityComponentVirtualImpl = Serialize::SerializableUnit<T, Base>;

        template <typename T, typename Base = EntityComponentBase>
        struct EntityComponent : Serialize::SerializableUnit<T, Base> {
			using Serialize::SerializableUnit<T, Base>::SerializableUnit;

            std::string_view key() const override
            {
                return componentName();
            }

			static const char* componentName() {
                return _componentName<T>();
			}

        };

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, ...) \
    DLL_EXPORT_VARIABLE2(constexpr, const char *const, Engine::Scene::Entity::, _componentName, #Name, __VA_ARGS__)

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    DLL_EXPORT_VARIABLE2(, const Engine::Scene::Entity::EntityComponentCollector::ComponentRegistrator<Name>, Engine::Scene::Entity::, _reg, {}, Name)

#define ENTITYCOMPONENT_IMPL(Name, ...)                                                        \
    DLL_EXPORT_VARIABLE2(constexpr, const char *const, Engine::Scene::Entity::, _componentName, #Name, __VA_ARGS__); \
    DLL_EXPORT_VARIABLE2(, const Engine::Scene::Entity::EntityComponentCollector::ComponentRegistrator<__VA_ARGS__>, Engine::Scene::Entity::, _reg, {}, __VA_ARGS__)

    }
}
}
