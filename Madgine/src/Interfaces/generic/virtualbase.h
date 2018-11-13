#pragma once

namespace Engine
{
	template <class T>
	class VirtualBase : public virtual T
	{
	public:
		using T::T;
	};
}