#pragma once

#include "Modules/threading/taskguard.h"
#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        DLL_IMPORT_VARIABLE2(const std::string_view, _componentName, typename T);

        template <typename T, typename Base>
        using EntityComponentVirtualImpl = VirtualScope<T, Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>>;

        template <typename T>
        struct EntityComponent : VirtualScope<T, Serialize::SerializableUnit<T, EntityComponentComponent<T>>> {
            using VirtualScope<T, Serialize::SerializableUnit<T, EntityComponentComponent<T>>>::VirtualScope;

            const std::string_view &key() const override
            {
                return componentName();
            }

            static const std::string_view &componentName()
            {
                return _componentName<T>();
            }
        };

#define REGISTER_ENTITYCOMPONENT(Name, target) \
        Engine::Threading::TaskGuard __##Name##_guard { []() { Engine::Scene::Entity::sComponentsByName()[#Name] = target;}, []() { Engine::Scene::Entity::sComponentsByName().erase(#Name); } };

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, FullType)                                                               \
    DLL_EXPORT_VARIABLE2(constexpr, const std::string_view, Engine::Scene::Entity::, _componentName, #Name, FullType) \
    REGISTER_ENTITYCOMPONENT(Name, Engine::virtualIndexRef<FullType>())

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    VIRTUALUNIQUECOMPONENT(Name)

#define ENTITYCOMPONENT_IMPL(Name, FullType)                                                                           \
    DLL_EXPORT_VARIABLE2(constexpr, const std::string_view, Engine::Scene::Entity::, _componentName, #Name, FullType); \
    UNIQUECOMPONENT(FullType)                                                                                          \
    REGISTER_ENTITYCOMPONENT(Name, Engine::indexRef<FullType>())

    }
}
}
