#pragma once

#include "generic/container_traits.h"

namespace Engine
{
	template <class _Base, class _Store, template <class...> class Container = std::vector, class... _Ty>
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
			if constexpr (std::is_same_v<Container<F>, std::vector<F>>){
				mComponents.reserve(this->sComponents().size());
			}
			for (auto f : this->sComponents())
			{
				if (f) {
					container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(), f(std::forward<_Ty>(args)...));
				}
				else
				{
					container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(), nullptr);
				}
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

		static std::vector<void*> registeredComponentsHashes()
		{
			std::vector<void*> result;
			result.reserve(Store::sComponents().size());
			for (auto& f : Store::sComponents())
			{
				if (f) {
					result.push_back(&f);
				}
			}
			return result;
		}

		typename Container<std::unique_ptr<Base>>::const_iterator postCreate(void* hash, _Ty ... args)
		{
			auto fIt = std::find_if(this->sComponents().begin(), this->sComponents().end(),
			                        [=](const F& f) { return &f == hash; });
			return container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(),
			                                                                   (*fIt)(std::forward<_Ty>(args)...)).first;
		}

		template <class T>
		T &get()
		{
			auto it = mComponents.begin();
			std::advance(it, T::component_index());
			return static_cast<T&>(**it);
		}

		Base &get(size_t i)
		{
			auto it = mComponents.begin();
			std::advance(it, i);
			return **it;
		}

	protected:
		template <class T>
		static size_t registerComponent()
		{
			Store::sComponents().emplace_back([](_Ty ... args)
			{
				return std::unique_ptr<Base>(std::make_unique<T>(std::forward<_Ty>(args)...));
			});
			return Store::sComponents().size() - 1;
		}

		static void unregisterComponent(size_t i)
		{
			auto it = Store::sComponents().begin();
			std::advance(it, i);
			*it = F();
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
				mIterator = -1;
			}

			size_t index()
			{
				return mIterator;
			}

		private:
			size_t mIterator;
		};

	private:
		Container<std::unique_ptr<Base>> mComponents;
	};


	template <class Base, class... _Ty>
	class MADGINE_BASE_EXPORT BaseCreatorStore
	{
	protected:
		static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents();
	};

	#define BASE_COLLECTOR_IMPL(Collector) template<> \
	DLL_EXPORT std::vector<Collector::F>& Collector::Store::sComponents() \
		{ \
			static std::vector<Collector::F> dummy; \
			return dummy; \
		}	


	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using BaseUniqueComponentCollector = UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>;

}
