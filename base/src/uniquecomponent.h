#pragma once

#include "uniquecomponentcollector.h"

namespace Engine{

template <class T, template <class, class> class Collector, class Base, class Creator = StandardHeapCreator>
class UniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename Collector<Base, Creator>::template ComponentRegistrator<T> _reg;
};

template <class T, template <class, class> class Collector, class Base, class Creator = StandardHeapCreator>
typename Collector<Base, Creator>::template ComponentRegistrator<T> UniqueComponent<T, Collector, Base, Creator>::_reg;

template <class T, class Base, class Creator = StandardHeapCreator>
using BaseUniqueComponent = UniqueComponent<T, BaseUniqueComponentCollector, Base, Creator>;


}


