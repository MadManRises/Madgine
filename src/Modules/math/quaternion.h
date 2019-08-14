#pragma once

#include "vector3.h"
#include "matrix3.h"
#include "pi.h"

namespace Engine {

	struct Quaternion {

		Quaternion() : w(1.0f), v(Vector3::ZERO) {}
	
		Quaternion(float radian, const Vector3 &axis) :
			w(cosf(0.5f*radian)),
			v(axis)
		{
			v.normalise();
			v *= sinf(0.5f*radian);
		}

		static Quaternion FromRadian(const Vector3 &angles)
		{
			return Quaternion(angles.x, Vector3::UNIT_X) * Quaternion(angles.y, Vector3::UNIT_Y) * Quaternion(angles.z, Vector3::UNIT_Z);
		}

		static Quaternion FromDegrees(const Vector3 &angles)
		{
			return FromRadian(angles / 180.0f * PI);
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

		Quaternion inverse() const
		{
			Quaternion q{ *this };
			q.invert();
			return q;
		}

		Matrix3 toMatrix() const
		{
			return Matrix3{
				1 - 2 * (v.y * v.y + v.z * v.z),     2 * (v.x * v.y - v.z * w  ),     2 * (v.x * v.z + v.y * w  ),
					2 * (v.x * v.y + v.z * w  ), 1 - 2 * (v.x * v.x + v.z * v.z),     2 * (v.y * v.z - v.x * w  ),
					2 * (v.x * v.z - v.y * w  ),     2 * (v.y * v.z + v.x * w  ), 1 - 2 * (v.x * v.x + v.y * v.y)
			};
		}

		Vector3 operator *(const Vector3 &dir) const
		{
			return toMatrix() * dir;
		}

		bool operator==(const Quaternion &other) const
		{
			return w == other.w && v == other.v;
		}

		float w;
		Vector3 v;
	};

}