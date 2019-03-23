#pragma once

#include "Interfaces/serialize/container/set.h"
#include "scenecomponentbase.h"

namespace Engine
{
	namespace Scene
	{
		
		class SceneComponentSet : public Serialize::SerializableSet<std::unique_ptr<SceneComponentBase>>
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

	template <>
	struct container_traits_helper2<SceneComponentSet>
	{
		typedef container_traits<Serialize::SerializableSet,std::unique_ptr<SceneComponentBase>> type;
	};

}
