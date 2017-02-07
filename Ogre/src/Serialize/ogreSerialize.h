#pragma once

#include "Serialize\Streams\serializestream.h"
#include "templates.h"

namespace Engine {


	template <>
	struct externValueType<Ogre::Vector3> {
		const constexpr static bool value = true;
	};

	template <>
	struct externValueType<Ogre::Vector2> {
		const constexpr static bool value = true;
	};
	
	namespace Serialize {

		SerializeOutStream &operator << (SerializeOutStream &out, const Ogre::Quaternion &q);
		SerializeInStream &operator >> (SerializeInStream &in, Ogre::Quaternion &q);

	}
}
