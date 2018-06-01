#pragma once


namespace Engine
{
	template <class T, class Collector>
	class UniqueComponent : public Collector::Base
	{
	public:
		using Collector::Base::Base;

		static size_t component_index()
		{
			return _reg.index();
		}

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
