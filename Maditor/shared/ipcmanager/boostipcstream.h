#pragma once

#include "Madgine/serialize/streams/bufferedstream.h"
#include "boostipcbuffer.h"


namespace Maditor {
	namespace Shared {



		class MADITOR_SHARED_EXPORT BoostIPCStream : 
			public Engine::Serialize::BufferedInOutStream
			 {

		public:
			BoostIPCStream(SharedConnectionPtr &&conn, bool slave, Engine::Serialize::SerializeManager &mgr, Engine::Serialize::ParticipantId id = 0);
			BoostIPCStream(const BoostIPCStream &) = delete;
			BoostIPCStream(BoostIPCStream &&other, Engine::Serialize::SerializeManager &mgr);
			virtual ~BoostIPCStream();


		private:

			BoostIPCBuffer mBuffer;			

		};

		

	}
}

