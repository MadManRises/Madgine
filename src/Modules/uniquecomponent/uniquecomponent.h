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

	

	DLL_IMPORT_VARIABLE2(typename Collector::template ComponentRegistrator<T>, _reg, typename T, typename Collector);

#define UNIQUECOMPONENT(Name) DLL_EXPORT_VARIABLE2(, Name::Collector::ComponentRegistrator<Name>, Engine::, _reg, {}, Name, Name::Collector)
                              


	template <class T, class _Collector>
	class UniqueComponent : public _Collector::Base
	{
	public:
		using Collector = _Collector;

		using Collector::Base::Base;

		static size_t component_index()
		{
			return _reg<T, Collector>().index();
		}		
	};



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
        struct UniqueComponent : Collector::Base {
        using Collector::Base::Base;
		};

	template <class T>
	size_t component_index();
}

#define UNIQUECOMPONENT(Name) 

#endif