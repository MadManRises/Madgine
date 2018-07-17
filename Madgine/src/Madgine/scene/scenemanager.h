#pragma once

#include "../madgineobject.h"

#include "../scripting/types/scope.h"

#include "../serialize/toplevelserializableunit.h"

#include "scenecomponentcollector.h"

#include "../serialize/container/list.h"

#include "entity/entity.h"

#include "../serialize/container/noparent.h"
#include "../core/framelistener.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_BASE_EXPORT SceneManager : public Serialize::TopLevelSerializableUnit<SceneManager>,
		                                             public Scripting::Scope<SceneManager>,
		                                             public MadgineObject,
		public Core::FrameListener
		{
		public:
			SceneManager(App::Application &app);
			virtual ~SceneManager() = default;

			virtual bool init() final;
			virtual void finalize() final;

			void readState(Serialize::SerializeInStream& in) override;
			void writeState(Serialize::SerializeOutStream& out) const override;

			virtual bool frameRenderingQueued(float timeSinceLastFrame, Scene::ContextMask context) final;
			virtual bool frameFixedUpdate(float timeStep, ContextMask context) final;


			Entity::Entity* createEntity(const std::string& behaviour = "", const std::string& name = "",
			                             std::function<void(Entity::Entity&)> init = {});
			Entity::Entity* createLocalEntity(const std::string& behaviour = "", const std::string& name = "");
			Entity::Entity* findEntity(const std::string& name);
			std::list<Entity::Entity*> entities();
			void removeLater(Entity::Entity* e);


			Entity::Entity* makeLocalCopy(Entity::Entity& e);
			Entity::Entity* makeLocalCopy(Entity::Entity&& e);			

			void clear();

			template <class T>
			T &getComponent()
			{
				return mSceneComponents.get<T>();
			}
			SceneComponentBase &getComponent(size_t i);
			size_t getComponentCount();

			template <class T>
			T &getGlobalAPIComponent()
			{
				return static_cast<T&>(getGlobalAPIComponent(T::component_index()));
			}

			Scripting::GlobalAPIComponentBase &getGlobalAPIComponent(size_t i);

			

			template <class T>
			void connectStateLoaded(T& slot)
			{
				mStateLoadedSignal.connect(slot);
			}

			template <class T>
			void connectCleared(T& slot)
			{
				mClearedSignal.connect(slot);
			}


			template <class T>
			void connectEntitiesCallback(T& slot)
			{
				mEntities.connectCallback(slot);
			}

			App::Application &app() const;

		protected:

			KeyValueMapList maps() override;

		private:

			void removeQueuedEntities();

			std::string generateUniqueName();

			std::tuple<SceneManager &, bool, std::string> createNonLocalEntityData(const std::string& name);
			std::tuple<SceneManager &, bool, std::string> createEntityData(const std::string& name, bool local);

		private:
			App::Application &mApp;
			size_t mItemCount;

			SceneComponentCollector mSceneComponents;

			Serialize::ObservableList<Entity::Entity, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<&SceneManager::createNonLocalEntityData>> mEntities;
			std::list<Serialize::NoParentUnit<Entity::Entity>> mLocalEntities;
			std::list<Entity::Entity *> mEntityRemoveQueue;			

			SignalSlot::Signal<> mStateLoadedSignal;
			SignalSlot::Signal<> mClearedSignal;

			
		};

	}
}
