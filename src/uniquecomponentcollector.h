#pragma once

#include "generic/container_traits.h"
#include "plugins/pluginmanager.h"

namespace Engine
{

	template <class T>
	struct CollectorName;

	template <class _Base, class _Store, template <class...> class Container = std::vector, class... _Ty>
	class UniqueComponentCollector : _Store
	{
	public:
		typedef _Base Base;
		typedef std::function<std::unique_ptr<Base>(_Ty...)> F;
		typedef _Store Store;

		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;

		UniqueComponentCollector(const Plugins::PluginManager &pluginManager, _Ty ... args)
		{
			if constexpr (std::is_same_v<Container<F>, std::vector<F>>){
				mComponents.reserve(this->sComponents().size());
			}
			size_t count = this->sComponents().size();
			for (const std::pair<const std::string, Plugins::Plugin> &p : pluginManager) {
				const std::vector<F> *components = loadFromPlugin(&p.second);
				if (components)
					count += components->size();
			}
			mSortedComponents.reserve(count);
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
			for (const std::pair<const std::string, Plugins::Plugin> &p : pluginManager) {
				const std::vector<F> *components = loadFromPlugin(&p.second);
				if (components) {
					mPluginOffsets[&p.second] = mSortedComponents.size();
					for (auto f : *components) {
						if (f) {
							std::unique_ptr<Base> p = f(std::forward<_Ty>(args)...);
							mSortedComponents.push_back(p.get());
							container_traits<Container, std::unique_ptr<Base>>::emplace(mComponents, end(), std::move(p));
						}
					}
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

		/*static std::vector<void*> registeredComponentsHashes()
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
		}*/

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

	private:
		static const std::vector<F> *loadFromPlugin(const Plugins::Plugin *plugin) {
			typedef const std::vector<F> *StoreLoader();
			StoreLoader *loader = (StoreLoader*)plugin->getSymbol(name());
			return loader ? (*loader)() : nullptr;
		}

		static const char *name();

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
		std::map<const Plugins::Plugin*, size_t> mPluginOffsets;
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

#define COLLECTOR_NAME(Name, Collector) \
inline const char *Collector::name(){\
	return "pluginComponents" #Name;\
}


#ifdef PLUGIN_BUILD

	template <class Base, class... _Ty>
	class LocalCreatorStore
	{
	public:
		static std::vector<std::function<std::unique_ptr<Base>(_Ty ...)>>& sComponents() {
			static std::vector<std::function<std::unique_ptr<Base>(_Ty...)>> dummy;
			return dummy;
		}
	};

	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using BaseUniqueComponentCollector = UniqueComponentCollector<Base, LocalCreatorStore<Base, _Ty...>, Container, _Ty...>;

	


#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) \
	extern "C" DLL_EXPORT inline const std::vector<Collector::F> *pluginComponents ## Name(){ \
		return &Collector::Store::sComponents(); \
	} \
\
	COLLECTOR_NAME(Name, Collector)

#else

	template <class Base, template <class...> class Container = std::vector, class... _Ty>
	using BaseUniqueComponentCollector = UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>;

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) COLLECTOR_NAME(Name, Collector)

#endif

}
