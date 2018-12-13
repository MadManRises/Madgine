#include "../interfaceslib.h"

#include "pluginmanager.h"

#include "plugin.h"

#include "pluginsectionlistener.h"

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
			mSettings("Madgine_plugins.cfg")
		{
			assert(!sSingleton);
			sSingleton = this;

			if (!mSettings["State"]["CurrentSelectionFile"].empty()) {
				std::experimental::filesystem::path p = selectionFiles()[mSettings["State"]["CurrentSelectionFile"]];
				p /= mSettings["State"]["CurrentSelectionFile"] + ".cfg";
				mCurrentSelectionFile = Ini::IniFile(p);
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
				for (PluginListener *listener : mListeners) {
					setupListenerOnSectionAdded(listener, &pib.first->second);					
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
			if (mCurrentSelectionFile && !mLoadingCurrentSelectionFile) {
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
			assert(!mLoadingCurrentSelectionFile);
			if (mCurrentSelectionFile) {				
				mCurrentSelectionFile->loadFromDisk();
				mLoadingCurrentSelectionFile = true;
				for (std::pair<const std::string, Ini::IniSection> &sec : *mCurrentSelectionFile) {
					(*this)[sec.first].loadFromIni(sec.second);					
				}
				mLoadingCurrentSelectionFile = false;
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

		void PluginManager::addListener(PluginListener * listener)
		{
			mListeners.push_back(listener);
			for (std::pair<const std::string, PluginSection> &sec : mSections)
				setupListenerOnSectionAdded(listener, &sec.second);
		}

		void PluginManager::removeListener(PluginListener * listener)
		{
			mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
		}


		void PluginManager::setupListenerOnSectionAdded(PluginListener *listener, PluginSection *section)
		{
			section->addListener(listener);
		}

		void PluginManager::shutdownListenerAboutToRemoveSection(PluginListener *listener, PluginSection *section)
		{
			section->removeListener(listener);
		}


	}
}