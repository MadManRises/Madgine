#pragma once

namespace Engine
{
	namespace Threading 
	{

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

			friend struct DataLock;
			DataLock lock();

		private:
			void unlock();

		private:
			std::atomic<std::thread::id> mCurrentHolder;
			std::atomic_flag mConsidered = ATOMIC_FLAG_INIT;
		};


		struct DataLock
		{
			DataLock(DataMutex &mutex, bool holdsLock);
			DataLock(const DataLock &) = delete;
			DataLock(DataLock &&other);
			~DataLock();

		private:
			DataMutex &mMutex;
			bool mHoldsLock;
		};

		bool try_lockData_sorted(DataMutex **begin, DataMutex **end);

		template <typename... Ty>
		bool try_lockData(Ty*... args)
		{
			std::array<DataMutex*, sizeof...(Ty)> mutexes{ args... };
			std::sort(mutexes.begin(), mutexes.end());
			return try_lockData_sorted(mutexes.begin(), mutexes.end());
		}

	}
}