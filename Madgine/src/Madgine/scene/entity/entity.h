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

			public:
				Entity(const Entity&, bool local);
				Entity(Entity&&, bool local);

				Entity(SceneManager &sceneMgr, bool local, const std::string& name, const Scripting::LuaTable &behavior = {});
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

				

				void writeState(Serialize::SerializeOutStream& of) const override;
				void readState(Serialize::SerializeInStream& ifs) override;
				void writeCreationData(Serialize::SerializeOutStream& of) const override;

				SceneManager& sceneMgr(bool = true) const;

				bool isLocal() const;

				template <class T>
				T &getSceneComponent(bool init = true)
				{
					return static_cast<T&>(getSceneComponent(T::component_index(), init));
				}

				SceneComponentBase &getSceneComponent(size_t i, bool = true);

				template <class T>
				T &getGlobalAPIComponent(bool init = true)
				{
					return static_cast<T&>(getGlobalAPIComponent(T::component_index(), init));
				}

				Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i, bool = true);

				App::Application &app(bool = true);

			protected:

				KeyValueMapList maps() override;

			private:

				std::tuple<std::unique_ptr<EntityComponentBase>> createComponentTuple(const std::string& name);

				EntityComponentBase* addComponentImpl(std::unique_ptr<EntityComponentBase>&& component);

				std::string mName;
				bool mLocal;

				Serialize::ObservableSet<std::unique_ptr<EntityComponentBase>, Serialize::ContainerPolicies::masterOnly, Serialize::
				                         ParentCreator<&Entity::createComponentTuple>>
				mComponents;

				SceneManager &mSceneManager;
			};



		}
	}
}
