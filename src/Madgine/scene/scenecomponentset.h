#pragma once

#include "Modules/serialize/container/set.h"
#include "scenecomponentbase.h"

namespace Engine
{
	namespace Scene
	{

		template <class T>
		class SceneComponentSet : public Serialize::SerializableSet<T>
		{
		public:
			struct traits : container_traits<Serialize::SerializableSet, T>
			{
				typedef SceneComponentSet<T> container;
			};

		protected:
			virtual void writeState(Serialize::SerializeOutStream& out) const override
			{
				for (const std::unique_ptr<SceneComponentBase>& component : *this)
				{
					out << component->key();
					component->writeId(out);
					component->writeState(out);
				}
				out << Serialize::EOLType();
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

}
