#pragma once

namespace Engine
{
	namespace GUI
	{
		struct WindowSizeRelVector
		{
			WindowSizeRelVector() :
				x(Ogre::Vector3::ZERO),
				y(Ogre::Vector3::ZERO)
			{
			}

			WindowSizeRelVector(const Ogre::Vector3& x, const Ogre::Vector3& y) :
				x(x), y(y)
			{
			}

			Ogre::Vector3 x, y; //abs, rel. W, rel. H

			operator bool() const
			{
				return !x.isZeroLength() || !y.isZeroLength();
			}

			Ogre::Vector2 operator *(const Ogre::Vector2& ref)
			{
				return {roundf(x[2] + x[0] * ref.x + x[1] * ref.y), roundf(y[2] + y[0] * ref.x + y[1] * ref.y)};
			}
		};
	}
}
