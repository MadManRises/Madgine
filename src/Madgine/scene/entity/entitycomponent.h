#pragma once

#include "entitycomponentbase.h"
#include "entitycomponentcollector.h"
#include "Interfaces/scripting/types/scope.h"

#include "Interfaces/serialize/streams/serializestream.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			template <class T, class Base = EntityComponentBase>
			class EntityComponentVirtualBase : public Scripting::Scope<T, Serialize::SerializableUnit<T, Base>>
			{
			public:
				using Scripting::Scope<T, Serialize::SerializableUnit<T, Base>>::Scope;

				const char* key() const override
				{
					return componentName();
				}

				static const char* componentName();

			private:
				virtual void writeCreationData(Serialize::SerializeOutStream& out) const override
				{
					out << componentName();
				}
			};

			template <class T, class Base>
			class EntityComponentVirtualImpl : public Serialize::SerializableUnit<T, Base>
			{
			public:
				using Serialize::SerializableUnit<T, Base>::SerializableUnit;

			private:
				virtual void __reg()
				{
					(void)_reg;
				}

				typedef EntityComponentCollector::ComponentRegistrator<T> _Reg;
				static const _Reg _reg;
			};

			template <class T, class Base = EntityComponentBase>
			class EntityComponent : public Scripting::Scope<T, Serialize::SerializableUnit<T, Base>>
			{
			public:
				using Scripting::Scope<T, Serialize::SerializableUnit<T, Base>>::Scope;

				const char* key() const override
				{
					return componentName();
				}

				static const char* componentName();

			private:
				virtual void writeCreationData(Serialize::SerializeOutStream& out) const override
				{
					out << componentName();
				}
				
				virtual void __reg()
				{
					(void)_reg;
				}				

				typedef EntityComponentCollector::ComponentRegistrator<T> _Reg;
				static const _Reg _reg;
			};


#define	ENTITYCOMPONENTVIRTUALBASE_IMPL(Name, ...) \
				template <> DLL_EXPORT const char * Engine::Scene::Entity::EntityComponentVirtualBase<__VA_ARGS__>::componentName(){return #Name;}

#define ENTITYCOMPONENTVIRTUALIMPL_IMPL(...) \
				template <> const Engine::Scene::Entity::EntityComponentVirtualImpl<__VA_ARGS__>::_Reg Engine::Scene::Entity::EntityComponentVirtualImpl<__VA_ARGS__>::_reg = {};

#define ENTITYCOMPONENT_IMPL(Name, ...) \
				template <> DLL_EXPORT const char * Engine::Scene::Entity::EntityComponent<__VA_ARGS__>::componentName(){return #Name;} \
				template <> const Engine::Scene::Entity::EntityComponent<__VA_ARGS__>::_Reg Engine::Scene::Entity::EntityComponent<__VA_ARGS__>::_reg = {};

		}
	}
}
