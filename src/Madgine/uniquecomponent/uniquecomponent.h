#pragma once

#ifndef STATIC_BUILD

namespace Engine
{

	template <class T, class Base>
	class VirtualUniqueComponentImpl : public Base
	{
	public:
		using Base::Base;

	private:
		virtual void __reg()
		{
			(void)_reg;
		}

		using Base::_preg;

		static class Inner
		{
		public:
			Inner()
			{
				_preg() = &reg;
			}

		private:
			typename Base::Collector::template ComponentRegistrator<T> reg;
		} _reg;
	};

	template <class T, class Base>
	typename VirtualUniqueComponentImpl<T, Base>::Inner VirtualUniqueComponentImpl<T, Base>::_reg;

	template <class T, class _Collector>
	class VirtualUniqueComponentBase : public _Collector::Base
	{
	public:
		using Collector = _Collector;

		using Collector::Base::Base;

		static size_t component_index()
		{
			return _preg()->index();
		}

	protected:

		static IndexHolder *& _preg(){ static IndexHolder *p = nullptr; return p; };
	};


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

	template <class T>
	size_t component_index() {
		return T::component_index();
	}
}

#else

namespace Engine {
	template <class T, class Base>
	using VirtualUniqueComponentImpl = Base;

	template <class T, class _Collector>
	using VirtualUniqueComponentBase = typename _Collector::Base;

	template <class T, class Collector>
	using UniqueComponent = typename Collector::Base;

	template <class T>
	size_t component_index();
}

#endif