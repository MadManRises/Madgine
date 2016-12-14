#include "madginelib.h"

#include "observable.h"
#include "serializableunit.h"
#include "Streams\serializestream.h"
#include "toplevelserializableunit.h"

namespace Engine {
		namespace Serialize {
			
			Observable::Observable(SerializableUnit * parent) :
				mParent(parent),
				mIndex(parent->addObservable(this)) {}

			std::list<SerializeOutStream*> Observable::getMasterMessageTargets(bool isAction)
			{
				std::list<SerializeOutStream*> result = mParent->getMasterMessageTargets(isAction);
				for (SerializeOutStream *out : result) {
					*out << mIndex;
				}
				return result;
			}

			bool Observable::isMaster()
			{
				return mParent->topLevel()->isMaster();
			}

		
			
		}
}