#include "madginelib.h"

#include "serializable.h"

#include "serializableunit.h"

namespace Engine {
		namespace Serialize {

			Serializable::Serializable(SerializableUnit * parent)
			{
				parent->addSerializableValue(this);
			}

			Serializable::Serializable()
			{
			}

			InvPtr Serializable::masterId()
			{
				return mMasterId;
			}

			void Serializable::setMasterId(InvPtr id)
			{
				mMasterId = id;
			}

		}
}