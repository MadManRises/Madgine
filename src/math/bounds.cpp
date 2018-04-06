#include "../ogrelib.h"
#include "bounds.h"

namespace Engine
{
	namespace Math
	{
		Bounds::Bounds(float left, float bottom, float right, float top) :
			mLeft(left),
			mBottom(bottom),
			mRight(right),
			mTop(top)

		{
		}

		Bounds::Bounds(const Ogre::Vector3& min, const Ogre::Vector3& max) :
			mLeft(min.x),
			mBottom(min.z),
			mRight(max.x),
			mTop(max.z)
		{
		}

		float Bounds::left() const
		{
			return mLeft;
		}

		float Bounds::top() const
		{
			return mTop;
		}

		float Bounds::right() const
		{
			return mRight;
		}

		float Bounds::bottom() const
		{
			return mBottom;
		}

		float Bounds::width() const
		{
			return mRight - mLeft;
		}

		float Bounds::height() const
		{
			return mTop - mBottom;
		}

		Ogre::Vector2 Bounds::size() const
		{
			return {width(), height()};
		}

		Ogre::Vector2 Bounds::bottomLeft() const
		{
			return {mLeft, mBottom};
		}

		std::array<Ogre::Vector2, 4> Bounds::corners() const
		{
			return {{{mLeft, mBottom}, {mLeft, mTop}, {mRight, mTop}, {mRight, mBottom}}};
		}

		Ogre::Vector2 Bounds::operator *(const Ogre::Vector2& v) const
		{
			return {mLeft + width() * v.x, mBottom + height() * v.y};
		}

		bool Bounds::isNull() const
		{
			return width() <= 0 || height() <= 0;
		}
	} // namespace Math
} // namespace Core
