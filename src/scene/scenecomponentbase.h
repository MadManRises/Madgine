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

			SceneComponentBase(SceneManagerBase &sceneMgr, ContextMask context = ContextMask::SceneContext);

			void update(float timeSinceLastFrame, ContextMask mask);
			void fixedUpdate(float timeStep, ContextMask mask);

			bool init() override;
			void finalize() override;

			void setEnabled(bool b);
			bool isEnabled() const;

			SceneManagerBase &sceneMgr() const;

			KeyValueMapList maps() override;

			virtual const char* key() const = 0;

			template <class T>
			T &getSceneComponent()
			{
				return static_cast<T&>(getSceneComponent(T::component_index()));
			}

			SceneComponentBase &getSceneComponent(size_t i);

		protected:
			virtual void update(float);
			virtual void fixedUpdate(float);

		private:
			const ContextMask mContext;

			bool mEnabled;

			SceneManagerBase &mSceneMgr;
		};

		


	}
}