#pragma once

namespace Engine
{
	namespace Threading 
	{

		struct DataLock;

		struct DataMutex 
		{
			enum ConsiderResult {
				UNAVAILABLE,
				ALREADY_CONSIDERED,
				CONSIDERED
			};
	
			bool isAvailable() const;
			ConsiderResult consider();
			void unconsider();
			
			bool lock();
            void unlock();			

		private:
			std::atomic<std::thread::id> mCurrentHolder;
			std::atomic_flag mConsidered = ATOMIC_FLAG_INIT;
		};


		struct DataLock
		{
			DataLock(DataMutex &mutex);
			DataLock(const DataLock &) = delete;
			DataLock(DataLock &&other);
			~DataLock();

		private:
			DataMutex &mMutex;
			bool mHoldsLock;
		};

		template <size_t S>
		bool try_lockData(std::array<DataMutex *, S> data) {
            std::sort(data.begin(), data.end());
            return try_lockData_sorted(data.begin(), data.end());
        }
		bool try_lockData_sorted(DataMutex * const *begin, DataMutex * const *end);      

	}
}