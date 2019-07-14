#include "../moduleslib.h"

#ifndef STATIC_BUILD

#include "pluginsection.h"

#include "plugin.h"

#include "pluginmanager.h"

#include "Interfaces/stringutil.h"

#include "Interfaces/exception.h"

#include "../threading/defaulttaskqueue.h"

#include "pluginlistener.h"

#include "Interfaces/filesystem/runtime.h"

#include "../keyvalue/keyvalue.h"

namespace Engine {
	namespace Plugins {

		PluginSection::PluginSection(PluginManager& mgr, const std::string& name) :
			mMgr(mgr),
			mName(name)
		{
			const std::regex e{ SHARED_LIB_PREFIX "Plugin_([a-zA-Z]*)_" + mName + "_([a-zA-Z]*)\\" SHARED_LIB_SUFFIX };
			std::smatch match;
			for (auto path : Filesystem::listSharedLibraries())
			{
				if (std::regex_match(path.str(), match, e)) {
					std::string project = match[1];
					std::string name = match[2];
					auto pib = mPlugins.try_emplace(name, name, project, path);
					assert(pib.second);
				}
			}
		}

		void PluginSection::setAtleastOne(bool atleastOne)
		{
			mAtleastOne = atleastOne;
			if (atleastOne) {
				if (mPlugins.empty())
					throw exception("No plugin available in Section tagged as atleastOne: "s + mName);
				for (std::pair<const std::string, Plugin> &p : mPlugins)
					if (p.second.isLoaded())
						return;
				if (!loadPlugin(&mPlugins.begin()->second))
					throw exception("Failed to load default Plugin for atleastOne Section: "s + mPlugins.begin()->first);
				mMgr.saveCurrentSelectionFile();
			}
		}

		bool PluginSection::isAtleastOne() const
		{
			return mAtleastOne;
		}

		void PluginSection::setExclusive(bool exclusive)
		{
			mExclusive = exclusive;
			if (exclusive) {
				bool foundOne = false;
				for (std::pair<const std::string, Plugin> &p : mPlugins) {
					if (p.second.isLoaded()) {
						if (!foundOne)
							foundOne = true;
						else
							if (!unloadPlugin(&p.second))
								throw exception("Failed to unload Plugin for exclusive Section: "s + p.first);
					}
				}
				mMgr.saveCurrentSelectionFile();
			}
		}

		bool PluginSection::isExclusive() const
		{
			return mExclusive;
		}

		bool PluginSection::isLoaded(const std::string & name) const
		{
			auto it = mPlugins.find(name);
			if (it != mPlugins.end())
				return it->second.isLoaded();
			return false;
		}

		bool PluginSection::loadPlugin(const std::string & name)
		{
			Plugin *plugin = getPlugin(name);
			if (!plugin)
				return false;
			return loadPlugin(plugin);
		}

		bool PluginSection::unloadPlugin(const std::string & name)
		{
			Plugin *plugin = getPlugin(name);
			if (!plugin)
				return false;
			return unloadPlugin(plugin);
		}

		bool PluginSection::loadPluginByFilename(const std::string & name)
		{
			auto pib = mPlugins.try_emplace(name, name);
			assert(pib.second);
			return loadPlugin(&pib.first->second);					
		}

		void PluginSection::addListener(PluginListener * listener)
		{
			std::unique_lock lock(mMgr.mListenersMutex);
			mListeners.push_back(listener);
			for (const std::pair<const std::string, Plugins::Plugin> &p : *this) {
				if (p.second.isLoaded())
					listener->onPluginLoad(&p.second);
			}
		}

		void PluginSection::removeListener(PluginListener * listener)
		{
			std::unique_lock lock(mMgr.mListenersMutex);
			for (const std::pair<const std::string, Plugins::Plugin> &p : *this) {
				if (p.second.isLoaded())
					listener->aboutToUnloadPlugin(&p.second);
			}
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		Plugin * PluginSection::getPlugin(const std::string & name)
		{
			auto it = mPlugins.find(name);
			if (it == mPlugins.end())
				return nullptr;
			return &it->second;
		}

		bool PluginSection::loadPlugin(Plugin * p)
		{
			if (p->isLoaded())
				return true;

			std::unique_lock lock(mMgr.mListenersMutex);

			bool ok = true;
			Plugin *unloadExclusive = nullptr;
			if (mExclusive) {
				for (Plugin &p : kvValues(mPlugins)) {
					if (p.isLoaded()) {
						assert(!unloadExclusive);
						unloadExclusive = &p;						
						for (PluginListener *listener : mListeners)
							ok &= listener->aboutToUnloadPlugin(&p);
					}
				}
			}
						
			for (PluginListener *listener : mListeners)
				ok &= listener->aboutToLoadPlugin(p);
			if (ok) {
				if (unloadExclusive) {
					for (PluginListener *listener : mListeners)
						listener->onPluginUnload(unloadExclusive);
					ok = unloadExclusive->unload();
				}
				if (ok) {
					ok = p->load();
					if (ok)
						for (PluginListener *listener : mListeners)
							listener->onPluginLoad(p);
					mMgr.saveCurrentSelectionFile();
				}
			}
			else {
				Threading::DefaultTaskQueue::getSingleton().queue([this, p, unloadExclusive]() {
					std::unique_lock lock(mMgr.mListenersMutex);
					bool ok = true;
					if (unloadExclusive) {
						for (PluginListener *listener : mListeners)
							listener->onPluginUnload(unloadExclusive);
						ok = unloadExclusive->unload();
					}
					if (ok) {
						ok = p->load();
						if (ok)
							for (PluginListener *listener : mListeners)
								listener->onPluginLoad(p);
						mMgr.saveCurrentSelectionFile();
					}
				});
			}
			return ok;
		}

		bool PluginSection::unloadPlugin(Plugin * p)
		{
			assert(!mAtleastOne);
			std::unique_lock lock(mMgr.mListenersMutex);
			bool ok = true;
			for (PluginListener *listener : mListeners)
				ok &= listener->aboutToUnloadPlugin(p);
			if (ok) {
				ok = p->unload();
				if (ok) {
					for (PluginListener *listener : mListeners)
						listener->onPluginUnload(p);
					mMgr.saveCurrentSelectionFile();
				}
			}
			else {
				Threading::DefaultTaskQueue::getSingleton().queue([this, p]() {
					std::unique_lock lock(mMgr.mListenersMutex);
					if (p->unload()) {
						for (PluginListener *listener : mListeners)
							listener->onPluginUnload(p);
						mMgr.saveCurrentSelectionFile();
					}
				});
			}
			return ok;
		}

		std::map<std::string, Plugin>::const_iterator PluginSection::begin() const
		{
			return mPlugins.begin();
		}

		std::map<std::string, Plugin>::const_iterator PluginSection::end() const
		{
			return mPlugins.end();
		}

		void PluginSection::loadFromIni(Ini::IniSection & sec)
		{
			for (const std::pair<const std::string, std::string> &p : sec) {
				Plugin &plugin = mPlugins.at(p.first);
				bool result = p.second.empty() ? unloadPlugin(&plugin) : loadPlugin(&plugin);
				assert(result);
			}
		}

		const void* PluginSection::getUniqueSymbol(const std::string& name) const
		{
			const void *symbol = nullptr;
			for (const std::pair<const std::string, Plugin> &p : mPlugins)
			{
				const void *s = p.second.getSymbol(name);
				if (s)
				{
					if (symbol)
						throw 0;
					symbol = s;
				}
			}
			return symbol;
		}

	}
}

#endif