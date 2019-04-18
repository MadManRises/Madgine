#pragma once

#ifndef STATIC_BUILD

#include "indexholder.h"

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

		static class Inner
		{
		public:
			Inner()
			{
				Base::_preg() = &reg;
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

		static IndexHolder *& _preg() {
			static IndexHolder *dummy;
			return dummy;
		}
	};


	template <class T, class _Collector>
	class UniqueComponent : public _Collector::Base
	{
	public:
		using Collector = _Collector;

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

#if WINDOWS
#define UNIQUECOMPONENT(Name) template <> Name::Collector::ComponentRegistrator<Name> Engine::UniqueComponent<Name, Name::Collector>::_reg;
#else
#define UNIQUECOMPONENT(Name) template <> DLL_EXPORT Name::Collector::ComponentRegistrator<Name> Engine::UniqueComponent<Name, Name::Collector>::_reg; \
	template DLL_EXPORT Name::Collector::ComponentRegistrator<Name> Engine::UniqueComponent<Name, Name::Collector>::_reg;
#endif

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

#define UNIQUECOMPONENT(Name) 

#endif