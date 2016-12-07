#include "madginelib.h"

#include "observable.h"
#include "serializableunit.h"
#include "Streams\serializestream.h"

namespace Engine {
		namespace Serialize {
			
			Observable::Observable(SerializableUnit * parent) :
				mParent(parent),
				mIndex(parent->addObservable(this)) {}

			std::list<SerializeOutStream*> Observable::getMessageTargets(bool isAction)
			{
				std::list<SerializeOutStream*> result = mParent->getMessageTargets(isAction);
				for (SerializeOutStream *out : result) {
					*out << mIndex;
				}
				return result;
			}

		
			
		}
}