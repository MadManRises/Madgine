#include "../moduleslib.h"
#include "connectionstore.h"
#include "connection.h"

namespace Engine
{

	namespace Threading
	{
		ConnectionStore::ConnectionStore()
		{
		}

		void ConnectionStore::clear()
		{
			mBegin.reset();
		}

		ConnectionStore& ConnectionStore::globalStore()
		{
			static ConnectionStore store;
			return store;
		}

		


	}
}
