#pragma once

#include "MadgineObject.h"
#include "Scripting\Types\refscopetoplevelserializableunit.h"
#include "Scripting\Types\globalapi.h"


#include "Scene\scenecomponent.h"

namespace Engine {
	namespace Scene {

		class MADGINE_BASE_EXPORT SceneManager : public Singleton<SceneManager>,
			public Scripting::RefScopeTopLevelSerializableUnit,
			public MadgineObject<SceneManager>,
			public Scripting::GlobalAPI<SceneManager>
		{
		public:
			SceneManager();
			virtual ~SceneManager() = default;

			Scripting::Scope *createSceneArray(size_t size);
			Scripting::Scope *createSceneStruct();
			Scripting::Scope *createSceneList();

			virtual Entity::Entity *createEntity(const std::string &name = "", const std::string &mesh = "", const std::string &behaviour = "", const Scripting::ArgumentList &args = {}) = 0;
			virtual Entity::Entity *findEntity(const std::string &name) = 0;

			virtual void init() override;
			virtual void finalize() override;
			
			virtual void update(float timeSinceLastFrame, App::ContextMask context);

			virtual void clear() = 0;

			size_t getComponentCount();

			void saveComponentData(Serialize::SerializeOutStream &out) const;
			void loadComponentData(Serialize::SerializeInStream &in);


			////FOR LAUNCHER
			std::set<BaseSceneComponent*> getComponents();

		protected:
			std::string generateUniqueName();

		private:
			size_t mItemCount;

			UniqueComponentCollector<BaseSceneComponent> mSceneComponents;

		};


		

	}
}