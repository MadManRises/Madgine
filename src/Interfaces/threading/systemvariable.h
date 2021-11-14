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
                mValue.wait(initial);
			}

			operator T() const
			{
				return mValue;
			}

			SystemVariable<T, initial> &operator=(T t)
			{
				mValue = t;
				return *this;
			}

		private:
			std::atomic<T> mValue;
		};

	}
}