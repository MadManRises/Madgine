#pragma once

namespace Engine {
	namespace GUI {


		struct WindowSizeRelVector {
			WindowSizeRelVector() :
				x(Ogre::Vector3::ZERO),
				y(Ogre::Vector3::ZERO) {}

			WindowSizeRelVector(const Ogre::Vector3 &x, const Ogre::Vector3 &y) :
				x(x), y(y) {}

			Ogre::Vector3 x, y; //abs, rel. W, rel. H

			operator bool() {
				return !x.isZeroLength() || !y.isZeroLength();
			}

			Ogre::Vector2 operator * (const Ogre::Vector2 &ref) {
				return{ x[0] + x[1] * ref.x + x[2] * ref.y, y[0] + y[1] * ref.x + y[2] * ref.y };
			}
		};

	}
}