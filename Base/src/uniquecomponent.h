#pragma once

#include "uniquecomponentcollector.h"

namespace Engine{

template <class T, template <class> class Collector, class Collection, class Base>
class UniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename Collector<Collection>::template ComponentRegistrator<T> _reg;
};

template <class T, template <class> class Collector, class Collection, class Base>
typename Collector<Collection>::template ComponentRegistrator<T> UniqueComponent<T, Collector, Collection, Base>::_reg;

template <class T, class Collection, class Base = Collection>
using BaseUniqueComponent = UniqueComponent<T, BaseUniqueComponentCollector, Collection, Base>;


}


