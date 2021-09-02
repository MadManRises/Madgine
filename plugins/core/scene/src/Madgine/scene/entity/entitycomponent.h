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
        using VirtualEntityComponentBase = EntityComponentVirtualBase<T>;

        template <typename T, typename Base = EntityComponentBase>
        using EntityComponent = EntityComponentComponent<T, Base>;

#define ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, FullType) \
    COMPONENT_NAME(Name, FullType)            

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(Name) \
    VIRTUALUNIQUECOMPONENT(Name)

#define ENTITYCOMPONENT_IMPL(Name, FullType)                                   \
    NAMED_UNIQUECOMPONENT(Name, FullType)                                      \
    UNIQUECOMPONENT2(Engine::Scene::Entity::EntityComponentList<FullType>, _2)

    }
}
}
