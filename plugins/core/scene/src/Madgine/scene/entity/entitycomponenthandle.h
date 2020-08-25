#pragma once

#include "Modules/generic/container/generationvector.h"


namespace Engine {
namespace Scene {

    namespace Entity {

        template <typename T>
        struct EntityComponentHandle {
            mutable GenerationVectorIndex mIndex;

            bool operator!=(const EntityComponentHandle<T> &other) const
            {
                return mIndex != other.mIndex;
            }

            bool operator==(const EntityComponentHandle<T> &other) const
            {
                return mIndex == other.mIndex;
            }
        };

        template <typename T>
        struct EntityComponentOwningHandle {
            mutable GenerationVectorIndex mIndex;
        };

    }

}
}