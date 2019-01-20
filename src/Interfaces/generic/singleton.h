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
#ifndef __MINGW32__
			assert(sSingleton);
#endif
			sSingleton = nullptr;
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

}

#if WINDOWS
#	define SINGLETON_EXPORT
#else
#	define SINGLETON_EXPORT DLL_EXPORT
#endif

#define SINGLETON_IMPL(Class) template <> thread_local SINGLETON_EXPORT Class *Engine::Singleton<Class>::sSingleton = nullptr
