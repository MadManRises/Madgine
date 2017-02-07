#pragma once

#include "baseentitycomponent.h"
#include "entity.h"

namespace Engine{
namespace Scene {
namespace Entity {

template <class T>
class EntityComponent : public BaseEntityComponent, public Scripting::API<T>{
public:
	using BaseEntityComponent::BaseEntityComponent;

    virtual std::string getName() const override{
        return sComponentName;
    }

	virtual std::string key() const override {
		return sComponentName;
	}

	static const char * const componentName() {
		return sComponentName;
	}
	

private:
	virtual void __reg() {
		(void)_reg;
	}

    virtual bool hasComponentMethod(const std::string &name) const final{
		return hasMethod(name);
    }

    virtual ValueType execComponentMethod(const std::string &name, const Scripting::ArgumentList &args) final{
		return execMethod(name, args);
    }

	virtual void writeCreationData(Serialize::SerializeOutStream &out) const override {
		out << sComponentName;
	}
    
	static const char * const sComponentName;
    static const Entity::ComponentRegistrator<T> _reg;

};

template <class T>
const Entity::ComponentRegistrator<T> EntityComponent<T>::_reg;

}
}
}

