#pragma once

#include "contextmasks.h"

#include "../core/madgineobject.h"
#include "Interfaces/serialize/serializableunit.h"
#include "Interfaces/scripting/types/scopebase.h"

#include "Interfaces/reflection/classname.h"

#include "../uniquecomponent/uniquecomponent.h"

namespace Engine
{
	namespace Scene
	{
		
		class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public Scripting::ScopeBase,
			public Core::MadgineObject
		{
		public:
			virtual ~SceneComponentBase() = default;

			SceneComponentBase(SceneManager &sceneMgr, ContextMask context = ContextMask::SceneContext);

			void update(std::chrono::microseconds timeSinceLastFrame, ContextMask mask);
			void fixedUpdate(std::chrono::microseconds timeStep, ContextMask mask);


			void setEnabled(bool b);
			bool isEnabled() const;

			SceneManager &sceneMgr(bool = true) const;

			KeyValueMapList maps() override;

			virtual const char* key() const = 0;

			template <class T>
			T &getSceneComponent(bool init = true)
			{
				return static_cast<T&>(getSceneComponent(component_index<T>(), init));
			}

			SceneComponentBase &getSceneComponent(size_t i, bool = true);

			template <class T>
			T &getGlobalAPIComponent(bool init = true)
			{
				return static_cast<T&>(getGlobalAPIComponent(component_index<T>(), init));
			}

			App::GlobalAPIBase &getGlobalAPIComponent(size_t i, bool = true);
			SceneComponentBase &getSelf(bool = true);

		protected:
			virtual bool init() override;
			virtual void finalize() override;

			virtual void update(std::chrono::microseconds);
			virtual void fixedUpdate(std::chrono::microseconds);

		private:
			const ContextMask mContext;

			bool mEnabled;

			SceneManager &mSceneMgr;
		};

	}
}

RegisterType(Engine::Scene::SceneComponentBase);