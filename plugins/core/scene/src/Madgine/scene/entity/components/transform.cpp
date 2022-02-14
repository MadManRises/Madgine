#include "../../../scenelib.h"

#include "transform.h"

#include "Meta/keyvalue/metatable_impl.h"
#include "Meta/serialize/serializetable_impl.h"

#include "Meta/math/transformation.h"

#include "Modules/uniquecomponent/uniquecomponentcollector.h"



ENTITYCOMPONENT_IMPL(Transform, Engine::Scene::Entity::Transform);

METATABLE_BEGIN(Engine::Scene::Entity::Transform)
PROPERTY(Position, getPosition, setPosition)
PROPERTY(Scale, getScale, setScale)
PROPERTY(Orientation, getOrientation, setOrientation)
METATABLE_END(Engine::Scene::Entity::Transform)

SERIALIZETABLE_BEGIN(Engine::Scene::Entity::Transform)
FIELD(mParent)
FIELD(mPosition)
FIELD(mScale)
FIELD(mOrientation)
SERIALIZETABLE_END(Engine::Scene::Entity::Transform)

namespace Engine {

namespace Scene {
    namespace Entity {

        Matrix4 Transform::matrix() const
        {
            return TransformMatrix(mPosition, mScale, mOrientation);
        }

        Matrix4 Transform::worldMatrix() const
        {
            return parentMatrix() * matrix();
        }

        Matrix4 Transform::parentMatrix() const
        {
            if (mParent)
                return mParent->worldMatrix();
            else
                return Matrix4::IDENTITY;
        }

        void Transform::setParent(Transform *parent)
        {
            if (parent == this)
                return;
            Transform *ptr = parent;
            while (ptr) {
                Transform *next = ptr->mParent;
                if (next == this) {
                    ptr->setParent(nullptr);
                    ptr = nullptr;
                } else {
                    ptr = next;
                }
            }
            mParent = parent;
        }

        Transform *Transform::parent() const
        {
            return mParent;
        }

        const Vector3 &Transform::getPosition() const
        {
            return mPosition;
        }
        const Vector3 &Transform::getScale() const
        {
            return mScale;
        }
        const Quaternion &Transform::getOrientation() const
        {
            return mOrientation;
        }
        void Transform::setPosition(const Vector3 &position)
        {
            mPosition = position;
        }
        void Transform::setScale(const Vector3 &scale)
        {
            mScale = scale;
        }
        void Transform::setOrientation(const Quaternion &orientation)
        {
            mOrientation = orientation;
        }
        void Transform::translate(const Vector3 &v)
        {
            mPosition += v;
        }
        void Transform::rotate(const Quaternion &q)
        {
            mOrientation *= q;
        }
    }
}
}

