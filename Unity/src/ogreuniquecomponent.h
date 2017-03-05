#pragma once

#include "ogreuniquecomponentcollector.h"

namespace Engine{

template <class T, class Base>
class OgreUniqueComponent : public Base , public Singleton<T>
{
public:
	using Base::Base;

private:
    virtual void __reg(){
		(void)_reg;
    }

private:
	static typename OgreUniqueComponentCollector<Base>::template ComponentRegistrator<T> _reg;
};

template <class T, class Base>
typename OgreUniqueComponentCollector<Base>::template ComponentRegistrator<T> OgreUniqueComponent<T, Base>::_reg;


}


