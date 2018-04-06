#pragma once

#include "generic/singleton.h"

namespace Engine
{
	template <class T, class Collector>
	class UniqueComponent : public Collector::Base, public Singleton<T>
	{
	public:
		using Collector::Base::Base;

	private:
		virtual void __reg()
		{
			(void)_reg;
		}

	private:
		static typename Collector::template ComponentRegistrator<T> _reg;
	};

	template <class T, class Collector>
	typename Collector::template ComponentRegistrator<T> UniqueComponent<T, Collector>::_reg;
}
