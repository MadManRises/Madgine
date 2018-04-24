#pragma once

#include "../../scripting/types/scope.h"

#include "entitycomponentbase.h"

#include "../../serialize/container/set.h"

namespace Engine
{
	namespace Scene
	{
		namespace Entity
		{
			class MADGINE_BASE_EXPORT Entity : public Serialize::SerializableUnit<Entity>, public Scripting::Scope<Entity>
			{
			private:
				typedef std::function<std::unique_ptr<EntityComponentBase>(Entity&, const Scripting::LuaTable&)> ComponentBuilder;

			public:
				Entity(const Entity&, bool local);
				Entity(Entity&&, bool local);

				Entity(SceneManagerBase &sceneMgr, bool local, const std::string& name, const std::string& behaviour = "");
				~Entity();

				void setup();

				void remove();

				const char* key() const override;

				template <class T>
				T* addComponent_t(const Scripting::LuaTable &table = {})
				{
					if (!hasComponent<T>())
						addComponent(T::componentName(), table);
					return getComponent<T>();
				}

				template <class T>
				void removeComponent_t()
				{
					if (hasComponent<T>())
						removeComponent(T::componentName());
				}

				template <class T>
				T* getComponent()
				{
					return static_cast<T*>(getComponent(T::componentName()));
				}

				EntityComponentBase* getComponent(const std::string& name);

				template <class T>
				bool hasComponent()
				{
					return hasComponent(T::componentName());
				}

				bool hasComponent(const std::string& name);

				void addComponent(const std::string& name, const Scripting::LuaTable& table = {});
				void removeComponent(const std::string& name);

				static bool existsComponent(const std::string& name);

				static std::set<std::string> registeredComponentNames();

				void writeState(Serialize::SerializeOutStream& of) const override;
				void readState(Serialize::SerializeInStream& ifs) override;
				void writeCreationData(Serialize::SerializeOutStream& of) const override;

				SceneManagerBase& sceneMgr() const;

				bool isLocal() const;

				template <class T>
				T &getSceneComponent()
				{
					return static_cast<T&>(getSceneComponent(T::component_index()));
				}

				SceneComponentBase &getSceneComponent(size_t i);

			protected:

				KeyValueMapList maps() override;

			private:

				template <class T>
				static std::unique_ptr<EntityComponentBase> createComponent_t(Entity& e, const Scripting::LuaTable &table = {})
				{
					return std::make_unique<T>(e, table);
				}

				std::unique_ptr<EntityComponentBase> createComponent(const std::string& name,
				                                                                 const Scripting::LuaTable& table = {});

				std::tuple<std::unique_ptr<EntityComponentBase>> createComponentTuple(const std::string& name);

				EntityComponentBase* addComponentImpl(std::unique_ptr<EntityComponentBase>&& component);

				template <class T>
				class ComponentRegistrator
				{
				public:
					ComponentRegistrator()
					{
						const std::string name = T::componentName();
						assert(sRegisteredComponentsByName().find(name) == sRegisteredComponentsByName().end());
						sRegisteredComponentsByName()[name] = &createComponent_t<T>;
					}

					~ComponentRegistrator()
					{
						const std::string name = T::componentName();
						assert(sRegisteredComponentsByName().find(name) != sRegisteredComponentsByName().end());
						sRegisteredComponentsByName().erase(name);
					}
				};

				static std::map<std::string, ComponentBuilder>& sRegisteredComponentsByName();

				template <class T, class Base>
				friend class EntityComponent;

				template <class T, class Base>
				friend class EntityComponentVirtualImpl;

				std::string mName;
				bool mLocal;

				Serialize::ObservableSet<std::unique_ptr<EntityComponentBase>, Serialize::ContainerPolicies::masterOnly, Serialize::
				                         ParentCreator<decltype(&Entity::createComponentTuple), &Entity::createComponentTuple>>
				mComponents;

				SceneManagerBase &mSceneManager;
			};
		}
	}
}
