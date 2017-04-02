#pragma once

#include "uniquecomponentcollector.h"

namespace Engine{

template <class T, class Base, template <class> class Collector = BaseUniqueComponentCollector>
class UniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename Collector<Base>::template ComponentRegistrator<T> _reg;
};

template <class T, class Base, template <class> class Collector>
typename Collector<Base>::template ComponentRegistrator<T> UniqueComponent<T, Base, Collector>::_reg;


}


