#include "../interfaceslib.h"
#include "connectionstore.h"
#include "connection.h"

namespace Engine
{

	namespace SignalSlot
	{
		std::shared_ptr<ConnectionBase> ConnectionStore::make_shared_connection(std::unique_ptr<ConnectionBase> &&conn)
		{
			return std::shared_ptr<ConnectionBase>(std::forward<std::unique_ptr<ConnectionBase>>(conn));
		}

		void ConnectionStore::disconnect(const const_iterator& where)
		{
			mConnections.erase(where);
		}

		void ConnectionStore::clear()
		{
			mConnections.clear();
		}

		ConnectionStore& ConnectionStore::globalStore()
		{
			static ConnectionStore store;
			return store;
		}

		


	}
}
