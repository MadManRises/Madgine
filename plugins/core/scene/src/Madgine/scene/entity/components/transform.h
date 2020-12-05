#pragma once

#include "../entitycomponent.h"
#include "Modules/math/matrix4.h"
#include "Modules/math/quaternion.h"
#include "Modules/math/vector3.h"

#include "Modules/serialize/container/serialized.h"

#include "../entitycomponenthandle.h"

#include "../entitycomponentptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {


        struct MADGINE_SCENE_EXPORT Transform : EntityComponent<Transform> {

            SERIALIZABLEUNIT(Transform);

            using EntityComponent<Transform>::EntityComponent;

            Transform(Transform &&) = default;

            Transform &operator=(Transform &&) = default;

            void finalize(const EntityPtr &e);

            const Vector3 &getPosition() const;
            const Vector3 &getScale() const;
            const Quaternion &getOrientation() const;

            void setPosition(const Vector3 &position);
            void setScale(const Vector3 &scale);
            void setOrientation(const Quaternion &orientation);

            void translate(const Vector3 &v);
            void rotate(const Quaternion &q);

            //KeyValueMapList maps() override;

            Matrix4 matrix() const;
            Matrix4 worldMatrix(const EntityComponentList<Transform> &transforms) const;
            Matrix4 parentMatrix(const EntityComponentList<Transform> &transforms) const;

            void updateParent(const EntityComponentList<Transform> &transforms) const;
            void setParent(const EntityComponentPtr<Transform> &parent);
            const EntityComponentHandle<Transform> &parent() const;

        private:
            Serialize::Serialized<Vector3> mPosition;
            Vector3 mScale = Vector3::UNIT_SCALE;
            Quaternion mOrientation;
            EntityComponentHandle<Transform> mParent;
        };

        template <>
        struct MADGINE_SCENE_EXPORT EntityComponentPtr<Transform> : EntityComponentPtrBase<Transform> {
            using EntityComponentPtrBase<Transform>::EntityComponentPtrBase;

            Matrix4 worldMatrix() const;
            Matrix4 parentMatrix() const;
        };

        using TransformPtr = EntityComponentPtr<Transform>;

    }
}
}

RegisterType(Engine::Scene::Entity::Transform);