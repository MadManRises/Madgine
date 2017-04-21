
#include "madginelib.h"

#include "ogreSerialize.h"

#include "valuetype.h"


namespace Engine {

	template <>
	ValueType::ValueType(const Ogre::Vector3 &v) :
		ValueType(std::array<float, 3>{v.x, v.y, v.z}) {}

	template <>
	ValueType::ValueType(const Ogre::Quaternion &q) :
		ValueType(std::array<float, 4>{q.w, q.x, q.y, q.z}) {}

	template <>
	OGREMADGINE_EXPORT Ogre::Vector3 ValueType::as<Ogre::Vector3>() const {
		const std::array<float, 3> &a = as<std::array<float, 3>>();
		return{ a[0], a[1], a[2] };
	}

	template <>
	OGREMADGINE_EXPORT Ogre::Vector2 ValueType::as<Ogre::Vector2>() const {
		const std::array<float, 2> &a = as<std::array<float, 2>>();
		return{ a[0], a[1] };
	}

	template <>
	OGREMADGINE_EXPORT Ogre::Quaternion ValueType::as<Ogre::Quaternion>() const {
		const std::array<float, 4> &a = as<std::array<float, 4>>();
		return{ a[0], a[1], a[2], a[3] };
	}
	
}