#pragma once

#include "EntityComponentBase.h"
#include "entity.h"
#include "Scripting\Types\scope.h"

namespace Engine{
namespace Scene {
namespace Entity {

template <class T, class Base = EntityComponentBase, class... _Ty>
class EntityComponent : public Scripting::Scope<T, Base>{
public:
	using Scripting::Scope<T, Base>::Scope;

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

	virtual void writeCreationData(Serialize::SerializeOutStream &out) const override {
		out << sComponentName << creationArguments();
	}

	virtual size_t getSize() const override final {
		return sizeof(T);
	}
    
	static const char * const sComponentName;
    static const Entity::ComponentRegistrator<T, _Ty...> _reg;

};

template <class T, class Base, class... _Ty>
const Entity::ComponentRegistrator<T, _Ty...> EntityComponent<T, Base, _Ty...>::_reg;

}
}
}


