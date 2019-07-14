#include "../moduleslib.h"
#include "bounds.h"

#include "vector2.h"
#include "vector3.h"

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

		Bounds::Bounds(const Vector3& min, const Vector3& max) :
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

		Vector2 Bounds::size() const
		{
			return {width(), height()};
		}

		Vector2 Bounds::bottomLeft() const
		{
			return {mLeft, mBottom};
		}

		std::array<Vector2, 4> Bounds::corners() const
		{
			return {{{mLeft, mBottom}, {mLeft, mTop}, {mRight, mTop}, {mRight, mBottom}}};
		}

		Vector2 Bounds::operator *(const Vector2& v) const
		{
			return {mLeft + width() * v.x, mBottom + height() * v.y};
		}

		bool Bounds::isNull() const
		{
			return width() <= 0 || height() <= 0;
		}
	} // namespace Math
} // namespace Core
