#include "maditorsharedlib.h"

#include "boostipcstream.h"

namespace Maditor {
	namespace Shared {

		BoostIPCStream::BoostIPCStream(SharedConnectionPtr &&conn, bool slave, Engine::Serialize::SerializeManager &mgr, Engine::Serialize::ParticipantId id) :
			Stream(mgr, id),
			BufferedInOutStream(mBuffer, mgr, id),
			mBuffer(std::forward<SharedConnectionPtr>(conn), slave)			
		{
			
		}

		BoostIPCStream::BoostIPCStream(BoostIPCStream && other, Engine::Serialize::SerializeManager & mgr) :
			Stream(mgr, other.mId),
			BufferedInOutStream(mBuffer, mgr, other.mId),
			mBuffer(std::forward<BoostIPCBuffer>(other.mBuffer))
		{
			
		}

		BoostIPCStream::~BoostIPCStream()
		{
			
		}



		
	}
}
