#pragma once

#include "Madgine/scene/entity/components/transform.h"

#include "Interfaces/serialize/container/serialized.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_SERVER_EXPORT ServerTransform : public EntityComponentVirtualImpl<ServerTransform, Transform>
			{
			public:
				ServerTransform(Entity& entity, const Scripting::LuaTable& table = {});
				virtual ~ServerTransform();

				Vector3 getPosition() const;
				Vector3 getScale() const;
				Vector4 getOrientation() const;

				void setPosition(const Vector3& v);
				void setScale(const Vector3& scale);
				void setOrientation(const Vector4& orientation);

				void translate(const Vector3& v);
				void rotate(const Vector4& q);

			private:
				Serialize::Serialized<Vector3> mPosition;
				Vector3 mScale;
				Vector4 mOrientation;
			};
		}
	}
}
