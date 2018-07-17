#pragma once

#include "../serialize/container/set.h"
#include "scenecomponentbase.h"

namespace Engine
{
	namespace Scene
	{

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
					auto it = kvFind(*this, componentName);
					(*it)->readId(in);
					(*it)->readState(in);
				}
			}
		};

				
	}

	template <class T>
	struct container_traits<Scene::SceneComponentSet, T> : public container_traits<Serialize::SerializableSet, T>
	{
	};
}
