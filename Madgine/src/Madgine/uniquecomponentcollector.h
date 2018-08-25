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

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) \
	extern "C" DLL_EXPORT inline const std::vector<Collector::F> *pluginComponents ## Name(size_t baseIndex){ \
		Collector::setBaseIndex(baseIndex); \
		return &Collector::sComponents(); \
	} \
\
	COLLECTOR_NAME(Name, Collector)

#else

#define PLUGIN_COLLECTOR_EXPORT(Name, Collector) COLLECTOR_NAME(Name, Collector)

#endif


	class IndexHolder
	{
	public:
		virtual size_t index() = 0;
	};

	template <class _Base, template <class...> class Container = std::vector, class... _Ty>
	class UniqueComponentCollector
	{
	public:
		typedef _Base Base;
		typedef Collector_F<Base, _Ty...> F;

		UniqueComponentCollector(const UniqueComponentCollector&) = delete;
		void operator=(const UniqueComponentCollector&) = delete;

		UniqueComponentCollector(const Plugins::PluginManager &pluginManager, _Ty ... args)
		{
			size_t count = sComponents().size();
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
			for (auto f : sComponents())
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

#ifndef PLUGIN_BUILD
		DLL_EXPORT 
#endif
		static std::vector<Collector_F<Base, _Ty...>>& sComponents();

		static void setBaseIndex(size_t index)
		{
			sBaseIndex = index;
		}

	protected:
		template <class T>
		static size_t registerComponent()
		{
			sComponents().emplace_back([](_Ty ... args)
			{
				return std::unique_ptr<Base>(std::make_unique<T>(std::forward<_Ty>(args)...));
			});
			return sComponents().size() - 1;
		}

		static void unregisterComponent(size_t i)
		{
			*std::next(sComponents().begin(), i) = F();
		}

		static inline size_t sBaseIndex = 0;

		static size_t baseIndex()
		{
			return sBaseIndex;
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

			size_t index() override
			{
				return mIndex + baseIndex();
			}

		private:
			size_t mIndex = -1;

		};

	private:
		Container<std::unique_ptr<Base>> mComponents;
		std::vector<Base*> mSortedComponents;
	};



	template <class _Base, template <class ...> class Container, class ... _Ty>
	std::vector<Collector_F<typename UniqueComponentCollector<_Base, Container, _Ty...>::Base, _Ty...>>& UniqueComponentCollector<_Base, Container, _Ty...>::sComponents()
	{
		static std::vector<Collector_F<Base, _Ty...>> dummy;
		return dummy;
	}



}
