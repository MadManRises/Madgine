#pragma once

#include "uniquecomponentcollector.h"

#include "generic/container_traits.h"
#include "plugins/pluginmanager.h"

namespace Engine
{

	template <class _Base, template <class...> class Container, class... _Ty>
	class UniqueComponentCollectorInstance : protected UniqueComponentCollector<_Base, _Ty...>
	{
	public:
		typedef typename UniqueComponentCollector<_Base, _Ty...>::F F;
		typedef typename UniqueComponentCollector<_Base, _Ty...>::Base Base;

		UniqueComponentCollectorInstance(_Ty ... args)
		{
			Plugins::PluginManager &pluginManager = Plugins::PluginManager::getSingleton();

			size_t count = sComponents().size();
#ifndef PLUGIN_BUILD
			for (const std::pair<const std::string, Plugins::PluginSection> &sec : pluginManager) {
				for (const std::pair<const std::string, Plugins::Plugin> &p : sec.second) {
					const std::vector<F> *components = loadFromPlugin(&p.second, count);
					if (components)
						count += components->size();
				}
			}
#endif
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
#ifndef PLUGIN_BUILD
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
#endif

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


	private:
		static const std::vector<F> *loadFromPlugin(const Plugins::Plugin *plugin, size_t baseIndex) {
			typedef const std::vector<F> *StoreLoader(size_t);
			StoreLoader *loader = (StoreLoader*)plugin->getSymbol(name());
			return loader ? (*loader)(baseIndex) : nullptr;
		}


	private:
		Container<std::unique_ptr<Base>> mComponents;
		std::vector<Base*> mSortedComponents;
	};

}