#pragma once

namespace Engine
{
	template <class T>
	struct VirtualBase : virtual T
	{	
		using T::T;
	};
}