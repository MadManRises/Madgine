#pragma once

#include "generic/container_traits.h"

namespace Engine
{
	template <class _Base, class _Store, template <class...> class Container = std::list, class... _Ty>
	class UniqueComponentCollector : _Store
	{
	public:
		typedef _Base Base;
		typedef std::function<std::unique_ptr<Base>(_Ty...)> F;
		typedef _Store Store;

		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;

		UniqueComponentCollector(_Ty ... args)
		{
			for (auto f : this->sComponents())
			{
				container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(), f(std::forward<_Ty>(args)...));
			}
		}

		typename Container<std::unique_ptr<Base>>::const_iterator begin() const
		{
			return mComponents.begin();
		}

		typename Container<std::unique_ptr<Base>>::const_iterator end() const
		{
			return mComponents.end();
		}

		size_t size()
		{
			return mComponents.size();
		}

		const Container<std::unique_ptr<Base>>& data() const
		{
			return mComponents;
		}

		static std::list<void*> registeredComponentsHashes()
		{
			std::list<void*> result;
			for (std::function<std::unique_ptr<Base>(_Ty ...)>& f : Store::sComponents())
			{
				result.push_back(&f);
			}
			return result;
		}

		typename Container<std::unique_ptr<Base>>::const_iterator postCreate(void* hash, _Ty ... args)
		{
			auto fIt = std::find_if(this->sComponents().begin(), this->sComponents().end(),
			                        [=](const std::function<std::unique_ptr<Base>(_Ty ...)>& f) { return &f == hash; });
			return container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(),
			                                                                   (*fIt)(std::forward<_Ty>(args)...)).first;
		}

	protected:
		template <class T>
		static typename std::list<std::function<std::unique_ptr<Base>(_Ty ...)>>::const_iterator registerComponent()
		{
			Store::sComponents().emplace_back([](_Ty ... args)
			{
				return std::unique_ptr<Base>(new T(std::forward<_Ty>(args)...));
			});
			auto it = Store::sComponents().end();
			--it;
			return it;
		}

		static void unregisterComponent(
			const typename std::list<std::function<std::unique_ptr<Base>(_Ty ...)>>::const_iterator& it)
		{
			Store::sComponents().erase(it);
		}

	public:
		template <class T>
		class ComponentRegistrator
		{
		public:
			ComponentRegistrator()
			{
				mIterator = registerComponent<T>();
			}

			~ComponentRegistrator()
			{
				unregisterComponent(mIterator);
			}

		private:
			typename std::list<std::function<std::unique_ptr<Base>(_Ty ...)>>::const_iterator mIterator;
		};

	private:
		Container<std::unique_ptr<Base>> mComponents;
	};


	template <class Base, class... _Ty>
	class MADGINE_BASE_EXPORT BaseCreatorStore
	{
	protected:
		static std::list<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents();
	};

	#define BASE_COLLECTOR_IMPL(Collector) template<> \
	DLL_EXPORT std::list<Collector::F>& Collector::Store::sComponents() \
		{ \
			static std::list<Collector::F> dummy; \
			return dummy; \
		}\
		SINGLETON_IMPL(Collector)
	


	template <class Base, template <class...> class Container = std::list, class... _Ty>
	class MADGINE_BASE_EXPORT BaseUniqueComponentCollector :
		public UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>,
		public Singleton<BaseUniqueComponentCollector<Base, Container, _Ty...>>
	{
		using UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>::UniqueComponentCollector;
	};
}
