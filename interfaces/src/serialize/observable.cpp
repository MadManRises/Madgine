#include "interfaceslib.h"

#include "observable.h"
#include "streams/bufferedstream.h"
#include "toplevelserializableunit.h"

namespace Engine {
		namespace Serialize {

			Observable::Observable() :
				mUnit(SerializableUnitBase::findParent(this)),
				mIndex(mUnit->addObservable(this))
			{
			}

			std::set<BufferedOutStream*, CompareStreamId> Observable::getMasterActionMessageTargets(const std::list<ParticipantId> &targets) const
			{
				std::set<BufferedOutStream*, CompareStreamId> result = mUnit->getMasterMessageTargets();
				for (BufferedOutStream *out : result) {
					out->beginMessage(mUnit, ACTION);
					*out << mIndex;
				}
				return result;
			}

			BufferedOutStream * Observable::getSlaveActionMessageTarget() const
			{
				BufferedOutStream *out = mUnit->getSlaveMessageTarget();
				out->beginMessage(mUnit, REQUEST);
				*out << mIndex;
				return out;
			}

			void Observable::beginActionResponseMessage(BufferedOutStream * stream)
			{
				stream->beginMessage(mUnit, ACTION);
				*stream << mIndex;
			}

			ParticipantId Observable::id()
			{
				return isMaster() ? mUnit->topLevel()->getLocalMasterParticipantId() : mUnit->topLevel()->getSlaveParticipantId();
			}

			bool Observable::isMaster() const
			{
				return !mUnit->isActive() || mUnit->topLevel()->isMaster();
			}

			SerializableUnitBase *Observable::parent() {
				return mUnit;
			}
		
			
		}
}
