#pragma once

#include "Modules/generic/container/generationvector.h"
#include "Modules/keyvalue/objectptr.h"
#include "Modules/generic/future.h"

namespace Engine {
namespace Scene {
    namespace Entity {

        struct MADGINE_SCENE_EXPORT EntityPtr {

            EntityPtr() = default;

            EntityPtr(const EntityPtr &other);
            EntityPtr(const typename GenerationVector<Entity>::iterator &it);

            ~EntityPtr();

            EntityPtr &operator=(const EntityPtr &other);

            Entity *operator->() const;

            SceneManager *sceneMgr() const;

            void update() const;

            operator bool() const;

            operator Entity *() const;

            bool operator==(const EntityPtr &other) const;
            bool operator!=(const typename GenerationVector<Entity>::iterator &it) const;

            EntityPtr &operator++();

            template <typename T>
            EntityComponentPtr<T> getComponent() const {
                return EntityComponentPtr<T> { getComponent(component_index<T>()) };
            }

            EntityComponentPtr<EntityComponentBase> getComponent(size_t i) const;

            template <typename T>
            Future<EntityComponentPtr<T>> addComponent(const ObjectPtr& table = {}) const {
                return addComponent(component_index<T>(), table).then([](EntityComponentPtr<EntityComponentBase> &&p) { return EntityComponentPtr<T> { std::move(p) }; });
            }

            Future<EntityComponentPtr<EntityComponentBase>> addComponent(size_t i, const ObjectPtr &table = {}) const;
            Future<EntityComponentPtr<EntityComponentBase>> addComponent(const std::string_view &name, const ObjectPtr &table = {}) const;

            typename GenerationVector<Entity>::iterator it();
            void remove();

        private:
            mutable GenerationVectorIndex mIndex;
            SceneManager *mSceneMgr = nullptr;
        };

    }
}
}