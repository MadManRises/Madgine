#include "../interfaceslib.h"

#ifndef STATIC_BUILD

#include "pluginmanager.h"

#include "plugin.h"

#include "../util/stringutil.h"

#include "../generic/templates.h"

#include "../filesystem/runtime.h"

#include "../generic/keyvalue.h"

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
				Filesystem::Path p = selectionFiles()[mSettings["State"]["CurrentSelectionFile"]];
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

		PluginSection & PluginManager::section(const std::string & name)
		{
			auto pib = mSections.try_emplace(name, *this, name);
			if (pib.second) {
				for (PluginListener *listener : mListeners) {
					setupListenerOnSectionAdded(listener, &pib.first->second);
				}
			}
			return pib.first->second;
		}

		PluginSection& PluginManager::operator[](const std::string& name) 
		{
			return section(name);
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
				for (auto &[name, section] : mSections)
				{
					Ini::IniSection &iniSec = (*mCurrentSelectionFile)[name];
					for (const std::pair<const std::string, Plugin> &p : section) {
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
				for (auto &[name, section] : *mCurrentSelectionFile) {
					if (name == "Core")
						continue;
					(*this)[name].loadFromIni(section);					
				}
				mLoadingCurrentSelectionFile = false;
			}
		}

		Ini::IniSection & PluginManager::selectionFiles()
		{
			return mSettings["SelectionFiles"];
		}

		Filesystem::Path PluginManager::currentSelectionPath()
		{
			return mCurrentSelectionFile ? mCurrentSelectionFile->path().parentPath() : "";
		}

		const std::string & PluginManager::currentSelectionName()
		{
			return mSettings["State"]["CurrentSelectionFile"];
		}

		void PluginManager::setCurrentSelection(const std::string & key, const Filesystem::Path &path)
		{
			mSettings["State"]["CurrentSelectionFile"] = key;
			selectionFiles()[key] = path.str();
			mSettings.saveToDisk();
			mCurrentSelectionFile = Ini::IniFile(path / (key + ".cfg"));	
			loadCurrentSelectionFile();		
			saveCurrentSelectionFile();
		}

		void PluginManager::addListener(PluginListener * listener)
		{
			{
				std::unique_lock lock(mListenersMutex);
				mListeners.push_back(listener);
			}
			for (PluginSection &section : kvValues(mSections))
				setupListenerOnSectionAdded(listener, &section);
		}

		void PluginManager::removeListener(PluginListener * listener)
		{
			{
				std::unique_lock lock(mListenersMutex);
				mListeners.erase(std::remove(mListeners.begin(), mListeners.end(), listener), mListeners.end());
			}
			for (PluginSection &section : kvValues(mSections))
				shutdownListenerAboutToRemoveSection(listener, &section);
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

#endif