#pragma once

#include "Modules/serialize/container/syncablecontainer.h"
#include "scenecomponentbase.h"

namespace Engine
{
	namespace Scene
	{

		template <typename OffsetPtr, class T>
            class SceneComponentSet : public Serialize::SerializableContainerImpl<OffsetPtr, std::set<T>>
		{
		public:
                    struct traits : Serialize::SerializableContainerImpl<OffsetPtr, std::set<T>>::traits
			{
                        typedef SceneComponentSet<OffsetPtr, T> container;
			};

		protected:
			void writeState(Serialize::SerializeOutStream& out) const
			{
                            const Serialize::SerializeTable *type = nullptr;

				for (const std::unique_ptr<SceneComponentBase>& component : *this)
				{
					out << component->key();
					component->writeId(out);
					type->writeBinary(component.get(), out);
				}
				out << Serialize::EOLType();
			}

			void readState(Serialize::SerializeInStream& in)
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
