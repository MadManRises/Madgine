#pragma once

#include "Modules/threading/taskguard.h"
#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T, typename Base>
        using VirtualEntityComponentImpl = VirtualScope<T, Serialize::SerializableUnit<T, VirtualUniqueComponentImpl<T, Base>>>;

        template <typename T>
        using VirtualEntityComponentBase = NamedComponent<T, EntityComponentVirtualBase<T>>;

        template <typename T>
        struct EntityComponent : VirtualScope<T, Serialize::SerializableUnit<T, NamedComponent<T, EntityComponentComponent<T>>>> {
            using VirtualScope<T, Serialize::SerializableUnit<T, NamedComponent<T, EntityComponentComponent<T>>>>::VirtualScope;

            const std::string_view &key() const override
            {
                return this->componentName();
            }            
        };

#define REGISTER_ENTITYCOMPONENT(Name, target) \
    Engine::Threading::TaskGuard __##Name##_guard { []() { Engine::Scene::Entity::sComponentsByName()[#Name] = target; }, []() { Engine::Scene::Entity::sComponentsByName().erase(#Name); } };

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, FullType) \
    COMPONENT_NAME(Name, FullType)                      \
    REGISTER_ENTITYCOMPONENT(Name, Engine::virtualIndexRef<FullType>())

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    VIRTUALUNIQUECOMPONENT(Name)

#define ENTITYCOMPONENT_IMPL(Name, FullType) \
    NAMED_UNIQUECOMPONENT(Name, FullType)    \
    REGISTER_ENTITYCOMPONENT(Name, Engine::indexRef<FullType>())

    }
}
}
