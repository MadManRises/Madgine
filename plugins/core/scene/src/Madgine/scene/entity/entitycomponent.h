#pragma once

#include "Modules/threading/taskguard.h"
#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

#include "entitycomponentlist.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T, typename Base>
        using VirtualEntityComponentImpl = VirtualScope<T, VirtualUniqueComponentImpl<T, Base>>;

        template <typename T>
        using VirtualEntityComponentBase = NamedComponent<T, EntityComponentVirtualBase<T>>;

        template <typename T, typename Base = EntityComponentBase>
        struct EntityComponent : VirtualScope<T, NamedComponent<T, EntityComponentComponent<T, Base>>> {
            using VirtualScope<T, NamedComponent<T, EntityComponentComponent<T, Base>>>::VirtualScope;

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

#define ENTITYCOMPONENT_IMPL(Name, FullType)                                   \
    NAMED_UNIQUECOMPONENT(Name, FullType)                                      \
    RegisterType(Engine::Scene::Entity::EntityComponentList<FullType>);        \
    UNIQUECOMPONENT2(Engine::Scene::Entity::EntityComponentList<FullType>, _2) \
    REGISTER_ENTITYCOMPONENT(Name, Engine::indexRef<FullType>())

    }
}
}
