#pragma once

#include "Modules/keyvalue/scopebase.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/uniquecomponent/uniquecomponent.h"

#include "Modules/keyvalue/objectptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {
        struct MADGINE_SCENE_EXPORT EntityComponentBase : Serialize::SerializableUnitBase, ScopeBase {        
            EntityComponentBase(Entity &entity, const ObjectPtr &initTable = {});
            virtual ~EntityComponentBase() = 0;

            virtual void init();
            virtual void finalize();

            void moveToEntity(Entity *ent);
            Entity &getEntity() const;

            virtual const char *key() const = 0;

            template <typename T>
            T *getComponent()
            {
                return static_cast<T *>(getComponent(T::componentName()));
            }

            EntityComponentBase *getComponent(const std::string &name);

            template <typename T>
            T *addComponent(const ObjectPtr &init = {})
            {
                return static_cast<T *>(addComponent(T::componentName(), init));
            }

            EntityComponentBase *addComponent(const std::string &name, const ObjectPtr &init);

            template <typename T>
            T &getSceneComponent(bool init = true)
            {
                return static_cast<T &>(getSceneComponent(T::component_index(), init));
            }

            SceneComponentBase &getSceneComponent(size_t i, bool = true);

            template <typename T>
            T &getGlobalAPIComponent(bool init = true)
            {
                return static_cast<T &>(getGlobalAPIComponent(T::component_index(), init));
            }

            App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);

            void writeCreationData(Serialize::SerializeOutStream &out) const;

        protected:
            const ObjectPtr &initTable();

        private:
            Entity *mEntity;
            ObjectPtr mInitTable;
        };
    }
}
}