#include "../interfaceslib.h"

#include "pluginsection.h"

#include "plugin.h"

#include "pluginmanager.h"

#include "../util/stringutil.h"

#include "../util/exception.h"

#include "../util/runtime.h"

#include "../signalslot/connectionmanager.h"

#include "pluginlistener.h"

namespace Engine {
	namespace Plugins {

		PluginSection::PluginSection(PluginManager& mgr, const std::string& name, const std::set<std::string> &fixedPlugins) :
			mMgr(mgr),
			mName(name)
		{
			for (const std::string &name : fixedPlugins)
			{
				auto pib = mPlugins.try_emplace(name, name);
				assert(pib.second);
				auto result = pib.first->second.load();
				assert(result);
			}
			for (auto &p : std::experimental::filesystem::directory_iterator(runtimePath()))
			{
				if (is_regular_file(p)) {
					std::experimental::filesystem::path path = p.path();
					std::string extension = path.extension().generic_string();
					std::string e = SHARED_LIB_SUFFIX;
					if (extension == SHARED_LIB_SUFFIX) {
						std::string filename = path.stem().generic_string();
						const std::string prefix = SHARED_LIB_PREFIX "Plugin_" + mMgr.project() + "_" + mName + "_";
						if (StringUtil::startsWith(filename, prefix)) {
							std::string name = filename.substr(prefix.size());
							auto pib = mPlugins.try_emplace(name, name, path);
							assert(pib.second);
						}
					}
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
			return loadPlugin(getPlugin(name));
		}

		bool PluginSection::unloadPlugin(const std::string & name)
		{
			return unloadPlugin(getPlugin(name));
		}

		void PluginSection::addListener(PluginListener * listener)
		{
			mListeners.push_back(listener);
			for (const std::pair<const std::string, Plugins::Plugin> &p : *this) {
				if (p.second.isLoaded())
					listener->onPluginLoad(&p.second);
			}
		}

		void PluginSection::removeListener(PluginListener * listener)
		{
			for (const std::pair<const std::string, Plugins::Plugin> &p : *this) {
				if (p.second.isLoaded())
					listener->aboutToUnloadPlugin(&p.second);
			}
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		Plugin * PluginSection::getPlugin(const std::string & name)
		{
			return &mPlugins.at(name);
		}

		bool PluginSection::loadPlugin(Plugin * p)
		{
			if (p->isLoaded())
				return true;

			bool ok = true;
			Plugin *unloadExclusive = nullptr;
			if (mExclusive) {
				for (std::pair<const std::string, Plugin> &p : mPlugins) {
					if (p.second.isLoaded()) {
						assert(!unloadExclusive);
						unloadExclusive = &p.second;
						for (PluginListener *listener : mListeners)
							ok &= listener->aboutToUnloadPlugin(&p.second);
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
				SignalSlot::ConnectionManager::getSingleton().queue([this, p, unloadExclusive]() {
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
				SignalSlot::ConnectionManager::getSingleton().queue([this, p]() {
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