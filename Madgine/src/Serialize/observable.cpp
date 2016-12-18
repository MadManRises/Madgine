#include "madginelib.h"

#include "observable.h"
#include "serializableunit.h"
#include "Streams\bufferedstream.h"
#include "toplevelserializableunit.h"

namespace Engine {
		namespace Serialize {
			
			Observable::Observable(SerializableUnit * parent) :
				mParent(parent),
				mIndex(parent->addObservable(this)) {}

			std::list<BufferedOutStream*> Observable::getMasterActionMessageTargets() const
			{
				std::list<BufferedOutStream*> result = mParent->getMasterMessageTargets(true);
				for (BufferedOutStream *out : result) {
					*out << mIndex;
				}
				return result;
			}

			BufferedOutStream * Observable::getSlaveActionMessageTarget() const
			{
				BufferedOutStream *out = mParent->getSlaveMessageTarget();
				*out << mIndex;
				return out;
			}

			bool Observable::isMaster() const
			{
				return mParent->topLevel()->isMaster();
			}

		
			
		}
}