#include "../interfaceslib.h"

#include "pluginmanager.h"

#include "plugin.h"

#include "pluginsectionlistener.h"

#include "../util/stringutil.h"

#if _WIN32
#define NOMINMAX
#include <Windows.h>
#include <psapi.h>
#elif __linux__
#include <link.h>
#endif

#include "../generic/templates.h"

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

			setupCoreSection();

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
					/*if (sec.first == "Core")
						continue;*/
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
					if (sec.first == "Core")
						continue;
					(*this)[sec.first].loadFromIni(sec.second);					
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


#if __linux__
		static int VisitModule(struct dl_phdr_info *info, size_t size, void *data) {
			Filesystem::Path file = Filesystem::Path(info->dlpi_name).filename();
			if (file.extension() == SHARED_LIB_SUFFIX)
			{
				std::string name = file.stem();
				if (StringUtil::startsWith(name, SHARED_LIB_PREFIX))
				{
					std::set<std::string> *modules = static_cast<std::set<std::string> *>(data);
					modules->insert(name.substr(strlen(SHARED_LIB_PREFIX)));
				}
			}
			return 0;
		}
#endif

		std::set<std::string> enumerateLoadedLibraries()
		{
			std::set<std::string> result;

#if _WIN32
			DWORD count;
			HMODULE modules[512];
			auto check = EnumProcessModules(GetCurrentProcess(), modules, array_size(modules), &count);
			assert(check);
			count /= sizeof(HMODULE);
			assert(count < array_size(modules));

			for (DWORD i = 0; i < count; ++i)
			{
				char buffer[512];
				auto check2 = GetModuleFileName(modules[i], buffer, sizeof(buffer));
				assert(check2);
				Filesystem::Path path = buffer;
				result.insert(path.stem());
			}
#elif __linux__
			dl_iterate_phdr(VisitModule, &result);
#else
#	error "Unsupported Platform!"
#endif

			return result;
		}


		void PluginManager::setupCoreSection()
		{
			const std::set<std::string> coreLibraries = { "Base", "Client" };
			const std::set<std::string> loadedLibraries = enumerateLoadedLibraries();
			std::set<std::string> loadedCoreLibraries;
			std::set_intersection(coreLibraries.begin(), coreLibraries.end(), loadedLibraries.begin(), loadedLibraries.end(),
				std::inserter(loadedCoreLibraries, loadedCoreLibraries.begin()));
			auto pib = mSections.try_emplace("Core", *this, "Core", loadedCoreLibraries);
			assert(pib.second);
		}

		

	}
}
