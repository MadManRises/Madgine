#pragma once

#include "Modules/generic/container/generationvector.h"
#include "Modules/uniquecomponent/uniquecomponent.h"


namespace Engine {
namespace Scene {

    namespace Entity {

        void entityComponentHelperWrite(Serialize::SerializeOutStream &out, GenerationContainerIndex &index, const char *name, SceneManager *mgr, size_t componentIndex);
        void entityComponentHelperRead(Serialize::SerializeInStream &in, GenerationContainerIndex &index, const char *name, SceneManager *mgr, size_t componentIndex);

        template <typename T>
        struct EntityComponentHandle {
            mutable GenerationContainerIndex mIndex;

            void readState(Serialize::SerializeInStream& in, const char* name, SceneManager* mgr) {
                
                entityComponentHelperRead(in, mIndex, name, mgr, Engine::component_index<T>());
            }

            void writeState(Serialize::SerializeOutStream& out, const char* name, SceneManager* mgr) const {
                entityComponentHelperWrite(out, mIndex, name, mgr, Engine::component_index<T>());
            }

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
            mutable GenerationContainerIndex mIndex;
        };

    }

}
}