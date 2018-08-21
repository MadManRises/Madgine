#pragma once

#include "../entitycomponent.h"
#include "../../../serialize/container/serializedmapper.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			
			class MADGINE_BASE_EXPORT Transform : public EntityComponentVirtualBase<Transform>
			{
			public:
				using EntityComponentVirtualBase<Transform>::EntityComponentVirtualBase;
				virtual ~Transform() = default;

				virtual Vector3 getPosition() const = 0;
				virtual Vector3 getScale() const = 0;
				virtual std::array<float, 4> getOrientation() const = 0;

				virtual void setPosition(const Vector3 &v) = 0;
				virtual void setScale(const Vector3 &scale) = 0;
				virtual void setOrientation(const std::array<float, 4> &orientation) = 0;

				virtual void translate(const Vector3 &v) = 0;
				virtual void rotate(const std::array<float, 4> &q) = 0;

			private:
				Vector3 getPosition2() const
				{
					return getPosition();
				}

				void setPosition2(const Vector3 &v)
				{
					setPosition(v);
				}

				Serialize::SerializedMapper<&Transform::getPosition2, 
					&Transform::setPosition2> mSerializedPosition;

			};

		}
	}
}
