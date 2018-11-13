#include "../interfaceslib.h"

#include "pluginmanager.h"

#include "../util/exception.h"

#include "../util/stringutil.h"

#include "../signalslot/connectionmanager.h"

namespace Engine
{
	namespace Plugins
	{

		PluginManager *PluginManager::sSingleton = nullptr;

		PluginManager& PluginManager::getSingleton()
		{
			assert(sSingleton);
			return *sSingleton;
		}

		PluginManager::PluginManager(const std::string &project) :
			mProject(project),
			mSettings("plugins.cfg")
		{
			assert(!sSingleton);
			sSingleton = this;

			if (!mSettings["State"]["CurrentSelectionFile"].empty()) {
				mCurrentSelectionFile = Ini::IniFile(selectionFiles()[mSettings["State"]["CurrentSelectionFile"]]);
				loadCurrentSelectionFile();
			}
		}

		bool PluginManager::isLoaded(const std::string & plugin) const
		{
			for (const std::pair<const std::string, PluginSection> &sec : mSections) {
				if (sec.second.isLoaded(plugin))
					return true;
			}
			return false;
		}

		const std::string& PluginManager::project() const
		{
			return mProject;
		}

		PluginSection& PluginManager::operator[](const std::string& name)
		{
			auto pib = mSections.try_emplace(name, *this, name);
			if (pib.second) {
				for (PluginSectionListener *listener : mListeners) {
					listener->onSectionAdded(&pib.first->second);
				}
			}
			return pib.first->second;
		}

		const PluginSection& PluginManager::at(const std::string& name) const
		{
			return mSections.at(name);
		}

		std::map<std::string, PluginSection>::const_iterator PluginManager::begin() const
		{
			return mSections.begin();
		}

		std::map<std::string, PluginSection>::const_iterator PluginManager::end() const
		{
			return mSections.end();
		}

		std::map<std::string, PluginSection>::iterator PluginManager::begin()
		{
			return mSections.begin();
		}

		std::map<std::string, PluginSection>::iterator PluginManager::end()
		{
			return mSections.end();
		}

		void PluginManager::saveCurrentSelectionFile()
		{
			if (mCurrentSelectionFile) {
				mCurrentSelectionFile->clear();
				for (const std::pair<const std::string, PluginSection> &sec : mSections) {
					Ini::IniSection &iniSec = (*mCurrentSelectionFile)[sec.first];
					for (const std::pair<const std::string, Plugin> &p : sec.second) {
						iniSec[p.first] = p.second.isLoaded() ? "On" : "";
					}
				}
				mCurrentSelectionFile->saveToDisk();
			}
		}

		void PluginManager::loadCurrentSelectionFile()
		{
			if (mCurrentSelectionFile) {
				mCurrentSelectionFile->loadFromDisk();
				for (std::pair<const std::string, Ini::IniSection> &sec : *mCurrentSelectionFile) {
					(*this)[sec.first].loadFromIni(sec.second);					
				}
			}
		}

		Ini::IniSection & PluginManager::selectionFiles()
		{
			return mSettings["SelectionFiles"];
		}

		std::experimental::filesystem::path PluginManager::currentSelectionPath()
		{
			return mCurrentSelectionFile ? mCurrentSelectionFile->path().parent_path() : "";
		}

		const std::string & PluginManager::currentSelectionName()
		{
			return mSettings["State"]["CurrentSelectionFile"];
		}

		void PluginManager::setCurrentSelection(const std::string & key, const std::experimental::filesystem::path & path)
		{
			mSettings["State"]["CurrentSelectionFile"] = key;
			selectionFiles()[key] = path.generic_string();
			mSettings.saveToDisk();
			mCurrentSelectionFile = Ini::IniFile(path / (key + ".cfg"));	
			loadCurrentSelectionFile();		
			saveCurrentSelectionFile();
		}

		void PluginManager::addListener(PluginSectionListener * listener)
		{
			mListeners.push_back(listener);
		}

		void PluginManager::removeListener(PluginSectionListener * listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		PluginSection::PluginSection(PluginManager& mgr, const std::string& name) :
		mMgr(mgr),
		mName(name)
		{
			for (auto &p : std::experimental::filesystem::directory_iterator(Plugin::runtimePath()))
			{
				if (is_regular_file(p)) {					
					std::experimental::filesystem::path path = p.path();
					std::string extension = path.extension().generic_string();					
					if (extension == ".dll") {
						std::string name = path.stem().generic_string();
						const std::string prefix = "Plugin_" + mMgr.project() + "_" + mName + "_";
						if (StringUtil::startsWith(name, prefix)) {
							auto pib = mPlugins.try_emplace(name.substr(prefix.size()), name);
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
				if (!mPlugins.begin()->second.load())
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
							if (!p.second.unload())
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
			Plugin *p = getPlugin(name);
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

		bool PluginSection::unloadPlugin(const std::string & name)
		{
			assert(!mAtleastOne);
			Plugin *p = getPlugin(name);
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

		void PluginSection::addListener(PluginListener * listener)
		{
			mListeners.push_back(listener);
		}

		void PluginSection::removeListener(PluginListener * listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}

		Plugin * PluginSection::getPlugin(const std::string & name)
		{
			return &mPlugins.at(name);
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
				bool result = p.second.empty() ? plugin.unload() : plugin.load();
				assert(result);
			}
		}

		void* PluginSection::getUniqueSymbol(const std::string& name) const
		{
			void *symbol = nullptr;
			for (const std::pair<const std::string, Plugin> &p : mPlugins)
			{
				void *s = p.second.getSymbol(name);
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
