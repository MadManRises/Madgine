#pragma once

#include "contextmasks.h"

#include "../core/madgineobject.h"
#include "Modules/serialize/serializableunit.h"
#include "Modules/keyvalue/scopebase.h"

#include "Modules/reflection/classname.h"

#include "Modules/uniquecomponent/uniquecomponent.h"

namespace Engine
{
	namespace Scene
	{
		
		class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public ScopeBase,
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

			//KeyValueMapList maps() override;

			const char* key() const;

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

			virtual const Core::MadgineObject *parent() const override;
			virtual App::Application &app(bool = true) override;

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