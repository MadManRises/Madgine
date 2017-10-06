#pragma once

#include "entitycomponentbase.h"
#include "entity.h"
#include "scripting/types/scope.h"

namespace Engine{
namespace Scene {
namespace Entity {

	template <class T, class Base = EntityComponentBase>
	class EntityComponent : public Scripting::Scope<T, Serialize::SerializableUnit<T, Base>> {
	public:
		using Scripting::Scope<T, Serialize::SerializableUnit<T, Base>>::Scope;

	const char *key() const override {
		return sComponentName;
	}

	static const char *componentName() {
		return sComponentName;
	}
	

private:
	virtual void __reg() {
		(void)_reg;
	}

	virtual void writeCreationData(Serialize::SerializeOutStream &out) const override {
		out << sComponentName;
	}
    
	static const char * const sComponentName;
    static const Entity::ComponentRegistrator<T> _reg;

};

template <class T, class Base>
const Entity::ComponentRegistrator<T> EntityComponent<T, Base>::_reg;

}
}
}


