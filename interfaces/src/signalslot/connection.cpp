#include "interfaceslib.h"
#include "connection.h"

namespace Engine
{
	namespace SignalSlot
	{
		ConnectionBase::ConnectionBase(ConnectionStore& store, const ConnectionStore::const_iterator& where) :
			mWhere(where),
			mStore(store)
		{
		}


		void ConnectionBase::disconnect()
		{
			mStore.disconnect(mWhere);
		}

		const std::shared_ptr<ConnectionBase>& ConnectionBase::ptr() const
		{
			return *mWhere;
		}

		ConnectionStore& ConnectionBase::store() const
		{
			return mStore;
		}
	}
}
