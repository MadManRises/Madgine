#pragma once

#include "madgineobject.h"

#include "scripting/types/scope.h"

#include "serialize/toplevelserializableunit.h"

#include "scene/scenecomponent.h"

#include "serialize/container/list.h"

#include "entity/entity.h"

#include "serialize/container/noparentunit.h"

namespace Engine {
	namespace Scene {



		class MADGINE_BASE_EXPORT SceneManagerBase : public Singleton<SceneManagerBase>,
			public Serialize::TopLevelSerializableUnit<SceneManagerBase>,
			public Scripting::Scope<SceneManagerBase>,
			public MadgineObject
		{
		public:
			SceneManagerBase();
			virtual ~SceneManagerBase() = default;

			Entity::Entity *createEntity(const std::string &behaviour = "", const std::string &name = "", std::function<void(Entity::Entity&)> init = {});
			Entity::Entity *createLocalEntity(const std::string &behaviour = "", const std::string &name = "");
			Entity::Entity *findEntity(const std::string &name);
			std::list<Entity::Entity*> entities();


			Entity::Entity *makeLocalCopy(Entity::Entity &e);
			Entity::Entity *makeLocalCopy(Entity::Entity &&e);
			
			virtual Light *createLight() = 0;
			virtual std::list<Light*> lights() = 0;

			virtual bool init() override;
			virtual void finalize() override;
			
			virtual void update(float timeSinceLastFrame, ContextMask context);
			virtual void fixedUpdate(float timeStep, ContextMask context);

			virtual void clear();

			size_t getComponentCount();


			virtual void readState(Serialize::SerializeInStream &in) override;
			virtual void writeState(Serialize::SerializeOutStream &out) const override;
			

			void removeLater(Entity::Entity *e);

			template <class T>
			void connectStateLoaded(T &slot) {
				mStateLoadedSignal.connect(slot);
			}

			template <class T>
			void connectCleared(T &slot) {
				mClearedSignal.connect(slot);
			}


			template <class T>
			void connectEntitiesCallback(T &slot) {
				mEntities.connectCallback(slot);
			}

			virtual const char *key() const;

		protected:		

			virtual KeyValueMapList maps() override;

		private:		

			void removeQueuedEntities();

			std::string generateUniqueName();

			std::tuple<SceneManagerBase *, std::string> createEntityData(const std::string &name);

		private:
			size_t mItemCount;

			SceneComponentCollector mSceneComponents;
			Serialize::ObservableList<Entity::Entity, Serialize::ContainerPolicy::masterOnly, Serialize::ParentCreator<decltype(&SceneManagerBase::createEntityData), &SceneManagerBase::createEntityData>> mEntities;
			std::list<Serialize::NoParentUnit<Entity::Entity>> mLocalEntities;
			std::list<Entity::Entity *> mEntityRemoveQueue;

			Engine::SignalSlot::Signal<> mStateLoadedSignal;
			Engine::SignalSlot::Signal<> mClearedSignal;


		};

		template <class T>
		class SceneManager : public Singleton<T>, public Serialize::SerializableUnit<T, SceneManagerBase> {
		public:
			using Serialize::SerializableUnit<T, SceneManagerBase>::SerializableUnit;

		};
		

	}
}