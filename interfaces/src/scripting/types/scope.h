#pragma once

#include "api.h"
#include "scopebase.h"


namespace Engine {
namespace Scripting {

	

template <class T, class Base = ScopeBase>
class Scope : public Base {

public:
	using Base::Base;
	
	virtual std::string getName() const override {
		return API<T>::sName;
	}

	virtual KeyValueMapList maps() override {
		return Base::maps().merge(API<T>::sAPI);
	}

};

}
}



