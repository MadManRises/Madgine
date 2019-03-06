#pragma once

namespace Engine
{
	namespace Math
	{
	
		
		inline bool RealEqual(float a, float b,
			float tolerance = std::numeric_limits<float>::epsilon()) {
			return std::abs(b - a) <= tolerance;
		}

		inline float Abs(float a)
		{
			return std::abs(a);
		}

		inline int Abs(int a) {
			return std::abs(a);
		}

		inline float Sqrt(float a)
		{
			return sqrtf(a);
		}

		inline float InvSqrt(float a)
		{
			return 1.0f / sqrtf(a);
		}

		inline int Sign(int i)
		{
			return (i == 0) ? 0 : ((i < 0) ? -1 : 1);
		}

	}
}