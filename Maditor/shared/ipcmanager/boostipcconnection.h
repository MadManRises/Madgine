#pragma once

#include "../sharedbase.h"

namespace Maditor {
	namespace Shared {



		class MADITOR_SHARED_EXPORT BoostIPCConnection
		{
		public:
			BoostIPCConnection(const std::string &prefix, boost::interprocess::managed_shared_memory::segment_manager *mgr);
			~BoostIPCConnection();

			const SharedString &prefix();

			static const constexpr size_t sMaxMessageSize = 256;

		private:

			SharedString mPrefix;

		};

		typedef boost::interprocess::managed_shared_ptr<BoostIPCConnection, boost::interprocess::managed_shared_memory>::type SharedConnectionPtr;

		typedef boost::interprocess::allocator<SharedConnectionPtr,
			boost::interprocess::managed_shared_memory::segment_manager> ConnectionPtrAllocator;
		typedef boost::interprocess::list<SharedConnectionPtr, ConnectionPtrAllocator> SharedConnectionQueue;

	}
}

