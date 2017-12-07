#pragma once


namespace Engine
{
	template <class T>
	class TEMPLATE_EXPORT Singleton
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

#define SINGLETON_IMPL(Class) template <> thread_local TEMPLATE_EXPORT Class *Engine::Singleton<Class>::sSingleton = nullptr
