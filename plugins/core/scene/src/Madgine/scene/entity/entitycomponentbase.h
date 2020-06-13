#pragma once

#include "Modules/keyvalue/virtualscopebase.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Modules/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableUnitBase, VirtualScopeBase {        
            EntityComponentBase(Entity &entity, const ObjectPtr &initTable = {});
            virtual ~EntityComponentBase() = 0;

            virtual void init();
            virtual void finalize();

            void moveToEntity(Entity *ent);
            Entity &getEntity() const;

            virtual const std::string_view &key() const = 0;

            template <typename T>
            T *getComponent()
            {
                return static_cast<T *>(getComponent(T::componentName()));
            }

            EntityComponentBase *getComponent(const std::string_view &name);

            template <typename T>
            T *addComponent(const ObjectPtr &init = {})
            {
                return static_cast<T *>(addComponent(component_index<T>(), T::componentName(), init));
            }

            EntityComponentBase *addComponent(const std::string_view &name, const ObjectPtr &init);
            EntityComponentBase *addComponent(size_t i, const std::string_view &name, const ObjectPtr &init);

            template <typename T>
            T &getSceneComponent(bool init = true)
            {
                return static_cast<T &>(getSceneComponent(component_index<T>(), init));
            }

            SceneComponentBase &getSceneComponent(size_t i, bool = true);

            template <typename T>
            T &getGlobalAPIComponent(bool init = true)
            {
                return static_cast<T &>(getGlobalAPIComponent(component_index<T>(), init));
            }

            App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

        protected:
            const ObjectPtr &initTable();

        private:
            Entity *mEntity;
            ObjectPtr mInitTable;
        };
    }
}
}

RegisterType(Engine::Scene::Entity::EntityComponentBase);