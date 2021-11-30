#pragma once

#include "Generic/replace.h"
#include "Generic/nulledptr.h"
#include "Generic/container/multicontainer.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        MADGINE_SCENE_EXPORT void relocateEntityComponent(Entity *entity, EntityComponentHandle<EntityComponentBase> index);

        template <template <typename...> typename C, typename T = Placeholder<0>>
        using EntityComponentContainerImpl = MultiContainer<C, T, NulledPtr<Entity>>;

        struct EntityComponentFreeListConfig {

            template <typename T>
            static bool isFree(ReferenceTuple<T, NulledPtr<Entity>> data)
            {
                return !get<1>(data);
            }

            template <typename T>
            static uintptr_t *getLocation(ReferenceTuple<T, NulledPtr<Entity>> data)
            {
                static_assert(sizeof(T) >= sizeof(uintptr_t));
                return &reinterpret_cast<uintptr_t &>(get<0>(data));
            }
        };

        struct EntityComponentRelocateFunctor {
            template <typename It>
            void operator()(const It &it, const It &begin)
            {
                using T = std::remove_reference_t<decltype(*get<0>(it))>;
                relocateEntityComponent(*get<1>(it), EntityComponentHandle<T> { std::distance(begin, it) });
            }
        };


    }
}
}