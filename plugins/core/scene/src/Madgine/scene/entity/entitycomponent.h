#pragma once

#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"

#include "entitycomponentlist.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        template <typename T, typename Base>
        using VirtualEntityComponentImpl = VirtualScope<T, VirtualUniqueComponentImpl<T, Base>>;

        template <typename T>
        using VirtualEntityComponentBase = EntityComponentVirtualBase<T>;

        template <typename T, typename Base = EntityComponentBase>
        using EntityComponent = EntityComponentComponent<T, Base>;

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, FullType) \
    COMPONENT_NAME(Name, FullType)

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    VIRTUALUNIQUECOMPONENT(Name)

#define ENTITYCOMPONENT_IMPL_EX(Name, FrontendType, FullType) \
    NAMED_UNIQUECOMPONENT_EX(Name, FrontendType, FullType)    \
    UNIQUECOMPONENT2(Engine::Scene::Entity::EntityComponentList<FullType>, _2)

#define ENTITYCOMPONENT_IMPL(Name, FullType) \
    ENTITYCOMPONENT_IMPL_EX(Name, FullType, FullType)

    }
}
}
