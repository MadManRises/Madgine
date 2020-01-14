#include "../moduleslib.h"
#include "connectionstore.h"
#include "connection.h"

namespace Engine
{

	namespace Threading
	{
		std::shared_ptr<ConnectionBase> ConnectionStore::make_shared_connection(std::unique_ptr<ConnectionBase> &&conn)
		{
			return std::shared_ptr<ConnectionBase>(std::forward<std::unique_ptr<ConnectionBase>>(conn));
		}

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
