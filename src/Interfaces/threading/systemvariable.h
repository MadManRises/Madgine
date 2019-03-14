#pragma once

namespace Engine
{
	namespace Threading
	{

		template <typename T, T initial = T{} >
		struct SystemVariable
		{
		public:
			SystemVariable() :
				mValue(initial) 
			{}

			SystemVariable(T t) :
				mValue(t)
			{}

			void wait()
			{
				if (mValue == initial)
				{
					std::unique_lock lock(mMutex);
					mCond.wait(lock, [this]() {return mValue != initial; });
				}
			}

			operator T() const
			{
				return mValue;
			}

			SystemVariable<T, initial> &operator=(T t)
			{
				mValue = t;
				mCond.notify_all();
				return *this;
			}

		private:
			std::atomic<T> mValue;
			std::mutex mMutex;
			std::condition_variable mCond;
		};

	}
}