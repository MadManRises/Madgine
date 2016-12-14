#include "madginelib.h"

#include "serializable.h"

#include "serializableunit.h"

namespace Engine {
		namespace Serialize {

			Serializable::Serializable(SerializableUnit * parent)
			{
			}

			Serializable::Serializable()
			{
			}

			void Serializable::applySerializableMap(const std::map<InvPtr, SerializableUnit*>& map)
			{
			}



		}
}