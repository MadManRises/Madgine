#pragma once

#include "../entitycomponent.h"
#include "Meta/math/matrix4.h"
#include "Meta/math/quaternion.h"
#include "Meta/math/vector3.h"

#include "../entitycomponentptr.h"

namespace Engine {
namespace Scene {
    namespace Entity {


        struct MADGINE_SCENE_EXPORT Transform : EntityComponent<Transform> {

            SERIALIZABLEUNIT(Transform);

            using Container = FreeListContainer<EntityComponentContainerImpl<std::deque>, EntityComponentFreeListConfig>;

            Transform() = default;
            using EntityComponent<Transform>::EntityComponent;

            Transform(Transform &&) = default;

            Transform &operator=(Transform &&) = default;

            const Vector3 &getPosition() const;
            const Vector3 &getScale() const;
            const Quaternion &getOrientation() const;

            void setPosition(const Vector3 &position);
            void setScale(const Vector3 &scale);
            void setOrientation(const Quaternion &orientation);

            void translate(const Vector3 &v);
            void rotate(const Quaternion &q);            

            Matrix4 matrix() const;
            Matrix4 worldMatrix() const;
            Matrix4 parentMatrix() const;

            void setParent(Transform *parent);
            Transform *parent() const;

        private:
            Vector3 mPosition;
            Vector3 mScale = Vector3::UNIT_SCALE;
            Quaternion mOrientation;
            Transform *mParent = nullptr;
        };

        using TransformPtr = EntityComponentPtr<Transform>;

    }
}
}

REGISTER_TYPE(Engine::Scene::Entity::Transform)
REGISTER_TYPE(Engine::Scene::Entity::EntityComponentList<Engine::Scene::Entity::Transform>)