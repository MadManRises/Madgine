#pragma once

#include "uniquecomponent.h"
#include "contextmasks.h"
#include "serialize/serializableunit.h"
#include "scripting/types/scope.h"
#include "madgineobject.h"

#include "serialize/container/set.h"

#include "uniquecomponentcollector.h"

namespace Engine
{
	namespace Scene
	{
		class MADGINE_BASE_EXPORT SceneComponentBase : public Serialize::SerializableUnitBase, public Scripting::ScopeBase,
		                                               public MadgineObject
		{
		public:
			virtual ~SceneComponentBase() = default;

			SceneComponentBase(SceneManagerBase* sceneMgr, ContextMask context = ContextMask::SceneContext);

			void update(float timeSinceLastFrame, ContextMask mask);
			void fixedUpdate(float timeStep, ContextMask mask);

			bool init() override;
			void finalize() override;

			void setEnabled(bool b);
			bool isEnabled() const;

			SceneManagerBase* sceneMgr() const;

			KeyValueMapList maps() override;

			virtual const char* key() const = 0;

		protected:
			virtual void update(float);
			virtual void fixedUpdate(float);

		private:
			const ContextMask mContext;

			bool mEnabled;

			SceneManagerBase* mSceneMgr;
		};

		template <class T>
		class SceneComponentSet : public Serialize::SerializableSet<T>
		{
		protected:
			virtual void writeState(Serialize::SerializeOutStream& out) const override
			{
				for (const std::unique_ptr<SceneComponentBase>& component : *this)
				{
					out << component->key();
					component->writeId(out);
					component->writeState(out);
				}
				out << ValueType::EOL();
			}

			virtual void readState(Serialize::SerializeInStream& in) override
			{
				std::string componentName;
				while (in.loopRead(componentName))
				{
					auto it = std::find_if(this->begin(), this->end(), [&](const std::unique_ptr<SceneComponentBase>& comp)
					{
						return comp->key() == componentName;
					});
					(*it)->readId(in);
					(*it)->readState(in);
				}
			}
		};


		using SceneComponentCollector = BaseUniqueComponentCollector<SceneComponentBase, SceneComponentSet, SceneManagerBase*>
		;

		template <class T>
		using SceneComponent = Serialize::SerializableUnit<
			T, Scripting::Scope<T, UniqueComponent<T, SceneComponentCollector>>>;
	}

	template <class T>
	struct container_traits<Scene::SceneComponentSet, T> : public container_traits<Serialize::SerializableSet, T>
	{
	};
}
