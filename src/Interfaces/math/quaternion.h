#pragma once

#include "vector3.h"
#include "matrix3.h"

namespace Engine {

	struct Quaternion {

		Quaternion() : w(1.0f), v(Vector3::ZERO) {}
	
		Quaternion(float radian, Vector3 axis) :
			w(cosf(0.5f*radian)),
			v(std::move(axis))
		{
			v.normalise();
			v *= sinf(0.5f*radian);
		}

		void operator *=(const Quaternion &other)
		{
			float newW = w * other.w - v.dotProduct(other.v);
			v = w * other.v + other.w * v + v.crossProduct(other.v);
			w = newW;			
		}

		Quaternion operator *(const Quaternion &other)
		{
			Quaternion q{ *this };
			q *= other;
			return q;			
		}

		/*void operator +=(const Quaternion &other)
		{
			w += other.w;
			v += other.v;			
		}

		Quaternion operator +(const Quaternion &other)
		{
			Quaternion q{ *this };
			q += other;
			return q;
		}

		void operator -=(const Quaternion &other)
		{
			w -= other.w;
			v -= other.v;			
		}

		Quaternion operator -(const Quaternion &other)
		{
			Quaternion q{ *this };
			q -= other;
			return q;
		}

		void operator *=(float c)
		{
			w *= c;
			v *= c;
		}

		Quaternion operator *(float c)
		{
			Quaternion q{ *this };
			q *= c;
			return q;
		}*/

		float dotProduct(const Quaternion &other)
		{
			return w * other.w + v.dotProduct(other.v);
		}

		void invert()
		{
			v *= -1;
		}

		Quaternion inverse()
		{
			Quaternion q{ *this };
			q.invert();
			return q;
		}

		Matrix3 toMatrix()
		{
			return Matrix3{
				1 - 2 * (v.y * v.y + v.z * v.z),     2 * (v.x * v.y - v.z * w  ),     2 * (v.x * v.z + v.y * w  ),
					2 * (v.x * v.y + v.z * w  ), 1 - 2 * (v.x * v.x + v.z * v.z),     2 * (v.y * v.z - v.x * w  ),
					2 * (v.x * v.z - v.y * w  ),     2 * (v.y * v.z + v.x * w  ), 1 - 2 * (v.x * v.x + v.y * v.y)
			};
		}

		Vector3 operator *(const Vector3 &dir)
		{
			return toMatrix() * dir;
		}

		float w;
		Vector3 v;
	};

}