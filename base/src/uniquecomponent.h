#pragma once

#include "uniquecomponentcollector.h"
#include "generic/singleton.h"

namespace Engine{

template <class T, template <class, class, class...> class Collector, class Base, class Creator = StandardHeapCreator, class... _Ty>
class UniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename Collector<Base, Creator, _Ty...>::template ComponentRegistrator<T> _reg;
};

template <class T, template <class, class, class...> class Collector, class Base, class Creator, class... _Ty>
typename Collector<Base, Creator, _Ty...>::template ComponentRegistrator<T> UniqueComponent<T, Collector, Base, Creator, _Ty...>::_reg;

template <class T, class Base, class Creator = StandardHeapCreator, class... _Ty>
using BaseUniqueComponent = UniqueComponent<T, BaseUniqueComponentCollector, Base, Creator, _Ty...>;


}


