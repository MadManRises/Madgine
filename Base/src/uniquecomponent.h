#pragma once

#include "uniquecomponentcollector.h"

namespace Engine{

template <class T, class Base>
class UniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename UniqueComponentCollector<Base>::template ComponentRegistrator<T> _reg;
};

template <class T, class Base>
typename UniqueComponentCollector<Base>::template ComponentRegistrator<T> UniqueComponent<T, Base>::_reg;


}


