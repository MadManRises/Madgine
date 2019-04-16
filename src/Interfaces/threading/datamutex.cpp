#include "../interfaceslib.h"

#include "datamutex.h"

namespace Engine
{
	namespace Threading
	{
		bool DataMutex::isAvailable() const
		{
			std::thread::id holder = mCurrentHolder;
			return holder == std::thread::id{} || holder == std::this_thread::get_id();
		}

		DataMutex::ConsiderResult DataMutex::consider()
		{
			bool alreadyConsidered = mConsidered.test_and_set();
			if (!isAvailable())
			{
				if (!alreadyConsidered)
					mConsidered.clear();
				return UNAVAILABLE;
			}
			else
			{
				return alreadyConsidered ? ALREADY_CONSIDERED : CONSIDERED;
			}			
		}

		void DataMutex::unconsider()
		{
			mConsidered.clear();
		}

		DataLock DataMutex::lock()
		{
			while (consider() != CONSIDERED);
			std::thread::id expected = {};
			std::thread::id self = std::this_thread::get_id();
			if (!mCurrentHolder.compare_exchange_strong(expected, self))
			{
				if (expected != self)
					std::terminate();				
			}
			unconsider();
			return DataLock(*this, expected != self);
		}

		void DataMutex::unlock()
		{
			std::thread::id expected = std::this_thread::get_id();
			if (!mCurrentHolder.compare_exchange_strong(expected, {}))
				std::terminate();
		}

		DataLock::DataLock(DataMutex & mutex, bool holdsLock) :
			mMutex(mutex),
			mHoldsLock(holdsLock)
		{
		}

		DataLock::DataLock(DataLock && other) :
			mMutex(other.mMutex),
			mHoldsLock(std::exchange(other.mHoldsLock, false))
		{
		}

		DataLock::~DataLock()
		{
			if (mHoldsLock)
				mMutex.unlock();
		}

		bool try_lockData_sorted(DataMutex **begin, DataMutex **end)
		{
			DataMutex **it = begin;

			bool success = true;
			bool checkAvailability = true;

			while (it != end && success)
			{
				if (checkAvailability)
				{
					checkAvailability = false;
					for (DataMutex **check = it; check != end && success; ++check)
					{
						if (!(*check)->isAvailable())
						{
							success = false;							
						}
					}
				}

				if (success)
				{
					switch ((*it)->consider())
					{
					case DataMutex::UNAVAILABLE:
						success = false;
						break;
					case DataMutex::ALREADY_CONSIDERED:
						checkAvailability = true;
						break;
					case DataMutex::CONSIDERED:
						++it;
						break;
					}
				}
			}
			
			//cleanup
			while (it != begin)
			{
				--it;
				(*it)->unconsider();
			}

			return success;

		}

	}
}