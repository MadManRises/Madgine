#pragma once

#include "../madgineobject.h"

#include "../scripting/types/scope.h"

#include "../serialize/toplevelserializableunit.h"

#include "scenecomponentcollector.h"

#include "../serialize/container/list.h"

#include "entity/entity.h"

#include "../serialize/container/noparent.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_BASE_EXPORT SceneManagerBase : public Serialize::TopLevelSerializableUnit<SceneManagerBase>,
		                                             public Scripting::Scope<SceneManagerBase>,
		                                             public MadgineObject
		{
		public:
			SceneManagerBase(App::Application &app);
			virtual ~SceneManagerBase() = default;

			Entity::Entity* createEntity(const std::string& behaviour = "", const std::string& name = "",
			                             std::function<void(Entity::Entity&)> init = {});
			Entity::Entity* createLocalEntity(const std::string& behaviour = "", const std::string& name = "");
			Entity::Entity* findEntity(const std::string& name);
			std::list<Entity::Entity*> entities();


			Entity::Entity* makeLocalCopy(Entity::Entity& e);
			Entity::Entity* makeLocalCopy(Entity::Entity&& e);

			virtual Light* createLight() = 0;
			virtual std::list<Light*> lights() = 0;

			bool init() override;
			void finalize() override;

			virtual void update(float timeSinceLastFrame, ContextMask context);
			virtual void fixedUpdate(float timeStep, ContextMask context);

			virtual void clear();

			template <class T>
			T &getComponent()
			{
				return mSceneComponents.get<T>();
			}
			SceneComponentBase &getComponent(size_t i);
			size_t getComponentCount();

			void readState(Serialize::SerializeInStream& in) override;
			void writeState(Serialize::SerializeOutStream& out) const override;


			void removeLater(Entity::Entity* e);

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

			const char* key() const override;

			App::Application &app() const;

		protected:

			KeyValueMapList maps() override;

		private:

			void removeQueuedEntities();

			std::string generateUniqueName();

			std::tuple<SceneManagerBase &, bool, std::string> createNonLocalEntityData(const std::string& name);
			std::tuple<SceneManagerBase &, bool, std::string> createEntityData(const std::string& name, bool local);

		private:
			App::Application &mApp;
			size_t mItemCount;

			SceneComponentCollector mSceneComponents;
			Serialize::ObservableList<Entity::Entity, Serialize::ContainerPolicies::masterOnly, Serialize::ParentCreator<
				                          decltype(&SceneManagerBase::createNonLocalEntityData), &SceneManagerBase::
				                          createNonLocalEntityData>> mEntities;
			std::list<Serialize::NoParentUnit<Entity::Entity>> mLocalEntities;
			std::list<Entity::Entity *> mEntityRemoveQueue;

			SignalSlot::Signal<> mStateLoadedSignal;
			SignalSlot::Signal<> mClearedSignal;

			
		};

	}
}
