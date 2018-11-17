#pragma once

namespace Engine
{
	namespace Math
	{
	
		
		static bool RealEqual(float a, float b,
			float tolerance = std::numeric_limits<float>::epsilon()) {
			return std::abs(b - a) <= tolerance;
		}

		static float Abs(float a)
		{
			return std::abs(a);
		}

		static int Abs(int a) {
			return std::abs(a);
		}

		static float Sqrt(float a)
		{
			return sqrtf(a);
		}

		static float InvSqrt(float a)
		{
			return 1.0f / sqrtf(a);
		}

		static int Sign(int i)
		{
			return (i == 0) ? 0 : ((i < 0) ? -1 : 1);
		}

	}
}