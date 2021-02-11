#pragma once

namespace Engine
{
template <typename T>
	struct VirtualBase : virtual T
	{	
		using T::T;
	};
}