#pragma once

#include "Modules/generic/container/generationvector.h"


namespace Engine {
namespace Scene {

    namespace Entity {

        struct EntityHandle {
            mutable GenerationContainerIndex mIndex;

            bool operator!=(const EntityHandle &other) const
            {
                return mIndex != other.mIndex;
            }

            bool operator==(const EntityHandle &other) const
            {
                return mIndex == other.mIndex;
            }
        };

    }

}
}