#pragma once

namespace Engine {

	template <class T>
	class Singleton {

	private:		
		Singleton(const Singleton<T> &) = delete;

		Singleton& operator=(const Singleton<T> &) = delete;
	public:

		static T &getSingleton() {
			assert(sSingleton);
			return *sSingleton;
		}

		static T *getSingletonPtr() {
			return sSingleton;
		}

	protected:
		Singleton() {
			assert(!sSingleton);
			sSingleton = static_cast<T*>(this);
		}

		~Singleton() {
			assert(sSingleton);
			sSingleton = 0;
		}

		static T *sSingleton;
	};

	template <class T>
	T *Singleton<T>::sSingleton = 0;

}