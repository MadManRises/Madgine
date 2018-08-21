#pragma once

#include "generic/container_traits.h"
#include "plugins/pluginmanager.h"

namespace Engine
{

	template <class Base, class... _Ty>
	using Collector_F = std::unique_ptr<Base>(*)(_Ty...);

#define COLLECTOR_NAME(Name, Collector) \
template<> inline const char *Collector::name(){\
	return "pluginComponents" #Name;\
}
	
#ifdef PLUGIN_BUILD

	template <class Base, class... _Ty>
	class LocalCreatorStore
	{
	public:
		static std::vector<Collector_F<Base, _Ty...>>& sComponents() {
			static std::vector<Collector_F<Base, _Ty...>> dummy;
			return dummy;
		}

		static size_t baseIndex() { return sBaseIndex; }

		static void setBaseIndex(size_t index) {
			sBaseIndex = index;
		}

	private:
		static inline size_t sBaseIndex = 0;
	};


#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) \
	extern "C" DLL_EXPORT inline const std::vector<Collector::F> *pluginComponents ## Name(size_t baseIndex){ \
		Collector::Store::setBaseIndex(baseIndex); \
		return &Collector::Store::sComponents(); \
	} \
\
	COLLECTOR_NAME(Name, Collector)

#else

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) COLLECTOR_NAME(Name, Collector)

#endif



	template <class T>
	struct CollectorName;

	class IndexHolder
	{
	public:
		virtual size_t index() = 0;
	};

	template <class _Base, class _Store, template <class...> class Container = std::vector, class... _Ty>
	class UniqueComponentCollector
	{
	public:
		typedef _Base Base;
		typedef Collector_F<Base, _Ty...> F;
#ifdef PLUGIN_BUILD
		typedef LocalCreatorStore<_Base, _Ty...> Store;
#else
		typedef _Store Store;
#endif

		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;

		UniqueComponentCollector(const Plugins::PluginManager &pluginManager, _Ty ... args)
		{
			//Necessary for dllexport
			(void)Store::baseIndex();
			size_t count = Store::sComponents().size();
			for (const std::pair<const std::string, Plugins::PluginSection> &sec : pluginManager) {
				for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
					const std::vector<F> *components = loadFromPlugin(&p.second, count);
					if (components)
						count += components->size();
				}
			}
			mSortedComponents.reserve(count);
			if constexpr (std::is_same_v<Container<F>, std::vector<F>>) {
				mComponents.reserve(count);
			}
			for (auto f : Store::sComponents())
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
			for (const std::pair<const std::string, Plugins::PluginSection> &sec : pluginManager) {
				for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
					const std::vector<F> *components = loadFromPlugin(&p.second, mSortedComponents.size());
					if (components) {
						for (auto f : *components) {
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

		template <class T>
		T &get()
		{
			return static_cast<T&>(get(T::component_index()));
		}

		Base &get(size_t i)
		{
			return **std::next(mSortedComponents.begin(), i);
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
			*std::next(Store::sComponents().begin(), i) = F();
		}

	private:
		static const std::vector<F> *loadFromPlugin(const Plugins::Plugin *plugin, size_t baseIndex) {
			typedef const std::vector<F> *StoreLoader(size_t);
			StoreLoader *loader = (StoreLoader*)plugin->getSymbol(name());
			return loader ? (*loader)(baseIndex) : nullptr;
		}

		static const char *name();

	public:
		template <class T>
		class ComponentRegistrator : public IndexHolder
		{
		public:
			ComponentRegistrator()
			{
				mIndex = registerComponent<T>();
			}

			~ComponentRegistrator()
			{
				unregisterComponent(mIndex);
				mIndex = -1;
			}

			virtual size_t index() override
			{
				return mIndex + Store::baseIndex();
			}

		private:
			size_t mIndex = -1;

		};

	private:
		Container<std::unique_ptr<Base>> mComponents;
		std::vector<Base*> mSortedComponents;
	};

	template <class Base, class... _Ty>
	class MADGINE_BASE_EXPORT BaseCreatorStore
	{
	public:
		static std::vector<Collector_F<Base, _Ty...>>& sComponents() {
			static std::vector<Collector_F<Base, _Ty...>> dummy;
			return dummy;
		}
	
		static constexpr size_t baseIndex() { return 0; }
	};



template <class Base, template <class...> class Container = std::vector, class... _Ty>
using BaseUniqueComponentCollector = UniqueComponentCollector<Base, BaseCreatorStore<Base, _Ty...>, Container, _Ty...>;

}
