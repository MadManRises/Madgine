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
			mSortedComponents.reserve(this->sComponents().size());
			for (auto f : this->sComponents())
			{
				if (f) {
					std::unique_ptr<Base> p = f(std::forward<_Ty>(args)...);
					mSortedComponents.push_back(p.get());
					container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(), std::move(p));
				}
				else
				{
					mSortedComponents.push_back(nullptr);
					container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end());
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

		/*typename Container<std::unique_ptr<Base>>::const_iterator postCreate(void* hash, _Ty ... args)
		{
			auto fIt = std::find_if(this->sComponents().begin(), this->sComponents().end(),
			                        [=](const F& f) { return &f == hash; });
			return container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(),
			                                                                   (*fIt)(std::forward<_Ty>(args)...)).first;
		}*/

		template <class T>
		T &get()
		{
			return static_cast<T&>(get(T::component_index()));
		}

		Base &get(size_t i)
		{
			auto it = mSortedComponents.begin();
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
		std::vector<Base*> mSortedComponents;
	};

	template <class Base, class... _Ty>
	class MADGINE_BASE_EXPORT BaseCreatorStore
	{
	protected:
		static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents() {
			static std::vector<std::function<std::unique_ptr<Base>(_Ty...)>> dummy;
			return dummy;
		}
	};

#ifdef PLUGIN_BUILD

	template <class Base, class... _Ty>
	class LocalCreatorStore
	{
	protected:
		static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents() {
			static std::vector<std::function<std::unique_ptr<Base>(_Ty...)>> dummy;
			return dummy;
		}
	};

	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using BaseUniqueComponentCollector = UniqueComponentCollector<Base, LocalCreatorStore<Base, _Ty...>, Container, _Ty...>;

#else

	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using BaseUniqueComponentCollector = UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>;

#endif

}
