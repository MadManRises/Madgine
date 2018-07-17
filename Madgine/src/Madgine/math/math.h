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

		static float Sqrt(float a)
		{
			return std::sqrtf(a);
		}

		static float InvSqrt(float a)
		{
			return 1.0f / std::sqrtf(a);
		}

	}
}