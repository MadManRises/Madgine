
#include "madginelib.h"

#include "ogreSerialize.h"

#include "valuetype.h"


namespace Engine {

	template <>
	ValueType::ValueType(const Ogre::Vector3 &v) :
		ValueType(std::array<float, 3>{v.x, v.y, v.z}) {}

	template <>
	MADGINE_EXPORT Ogre::Vector3 ValueType::as<Ogre::Vector3>() const {
		const std::array<float, 3> &a = as<std::array<float, 3>>();
		return{ a[0], a[1], a[2] };
	}

	template <>
	MADGINE_EXPORT Ogre::Vector2 ValueType::as<Ogre::Vector2>() const {
		const std::array<float, 2> &a = as<std::array<float, 2>>();
		return{ a[0], a[1] };
	}

	namespace Serialize {

		SerializeOutStream & operator<<(SerializeOutStream & out, const Ogre::Quaternion & q)
		{
			return out << *q.ptr() << Ogre::Vector3(q.ptr() + 1);
		}


		SerializeInStream & operator >> (SerializeInStream & in, Ogre::Quaternion & q)
		{
			Ogre::Vector3 v;
			Ogre::Real w;
			in >> w >> v;

			q = { w, v.x, v.y, v.z };

			return in;
		}

	}
}