#pragma once

namespace Engine
{
	template <class T>
	class Singleton
	{
	private:
		Singleton(const Singleton<T>&) = delete;

		Singleton& operator=(const Singleton<T>&) = delete;

	protected:
		Singleton()
		{
			assert(!sSingleton);
			sSingleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			assert(sSingleton);
			sSingleton = 0;
		}

	public:

		static T& getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		static T* getSingletonPtr()
		{
			return sSingleton;
		}

	private:
		static thread_local T* sSingleton;
	};

	template <class T>
	thread_local T* Singleton<T>::sSingleton = nullptr;
}
