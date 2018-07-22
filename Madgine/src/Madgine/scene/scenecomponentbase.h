#pragma once

#include "contextmasks.h"

#include "../madgineobject.h"
#include "../serialize/serializableunit.h"
#include "../scripting/types/scopebase.h"


namespace Engine
{
	namespace Scene
	{
		
		class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public Scripting::ScopeBase,
		                                               public MadgineObject
		{
		public:
			virtual ~SceneComponentBase() = default;

			SceneComponentBase(SceneManager &sceneMgr, ContextMask context = ContextMask::SceneContext);

			void update(float timeSinceLastFrame, ContextMask mask);
			void fixedUpdate(float timeStep, ContextMask mask);


			void setEnabled(bool b);
			bool isEnabled() const;

			SceneManager &sceneMgr(bool = true) const;

			KeyValueMapList maps() override;

			virtual const char* key() const = 0;

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
			SceneComponentBase &getSelf(bool = true);

		protected:
			virtual bool init() override;
			virtual void finalize() override;

			virtual void update(float);
			virtual void fixedUpdate(float);

		private:
			const ContextMask mContext;

			bool mEnabled;

			SceneManager &mSceneMgr;
		};

		


	}
}