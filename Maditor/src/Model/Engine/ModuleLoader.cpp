#include "madgineinclude.h"

#include "ModuleLoader.h"

#include "Model\Project\Project.h"


namespace Maditor {
	namespace Model {


		ModuleLoader::ModuleLoader() :
			mModules(0),
			mNeedReload(false),
			mInit(false)
		{
			connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &ModuleLoader::onFileChanged);
			connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, &ModuleLoader::onFolderChanged);
		}


		ModuleLoader::~ModuleLoader()
		{
			cleanup();
		}

		void ModuleLoader::onFileChanged(const QString &path) {
			QFileInfo f(path);
			auto it = mInstances.find(mModules->getModuleByName(f.baseName()));
			//if (it == mInstances.end())
			//	return;
			ModuleInstance &module = it->second;
			module.mExists = f.exists();
			if (module.mExists) {
				mReloadMutex.lock();
				module.mNeedReload = true;
				mNeedReload = true;
				mReloadMutex.unlock();
				qDebug() << path << "Changed!";
			}
			else {
				qDebug() << path << "was Deleted!";
			}
		}


		void ModuleLoader::onFolderChanged(const QString & path)
		{
			
			const QDir dir(path);

			QSet<QString> newEntryList = QSet<QString>::fromList(dir.entryList({ "*.dll" }, QDir::NoDotAndDotDot | QDir::Files));

			// Files that have been added
			QSet<QString> newFiles = newEntryList - mFiles;
			QStringList newFileList = newFiles.toList();

			//qDebug() << newEntryList << mFiles;

			// Update the current set
			mFiles = newEntryList;

			for (const QString &file : newFileList) {
				QFileInfo f(file);
				if (mModules->hasModule(f.baseName())) {
					ModuleInstance &module = mInstances.find(mModules->getModuleByName(f.baseName()))->second;
					mReloadMutex.lock();
					module.mExists = true;
					module.mNeedReload = true;
					mNeedReload = true;
					mReloadMutex.unlock();
				}
			}

		}


		void ModuleLoader::setup(const QString & binaryDir, const QString & runtimeDir, ModuleList *moduleList)
		{
			
			assert(!mInit);
			mInit = true;

			mRuntimeDir = runtimeDir;
			mBinaryDir = binaryDir;
			mModules = moduleList;

			QDir dir(binaryDir);
			mFiles = QSet<QString>::fromList(dir.entryList({"*.dll"}, QDir::NoDotAndDotDot | QDir::Files));

			mWatcher.addPath(binaryDir);

			
			mNeedReload = true;

			
			
			for (const std::unique_ptr<Module> &module : *mModules) {
				addModule(module.get());
			}

			update(false);

		}

		void ModuleLoader::cleanup()
		{
			if (!mBinaryDir.isEmpty())
				mWatcher.removePath(mBinaryDir);

			std::list<const Module*> reloadOrder;

			for (const std::pair<const Module* const, ModuleInstance> &p : mInstances) {
				p.first->fillReloadOrder(reloadOrder);
			}

			for (const Module *m : reloadOrder) {
				auto it = mInstances.find(m);
				ModuleInstance &instance = it->second;

				qDebug() << "Unloading" << m->name();
				unloadModule(instance);
				mWatcher.removePath(mBinaryDir + m->name() + ".dll");
			}

			mInstances.clear();
			
			mInit = false;
		}

		void ModuleLoader::update(bool callInit)
		{
			if (!mNeedReload)
				return;
			mReloadMutex.lock();
			mNeedReload = false;
			mReloadMutex.unlock();
			std::list<const Module*> reloadOrder;
			for (const std::pair<const Module* const, ModuleInstance> &p : mInstances){
				if (p.second.mNeedReload)
					p.first->fillReloadOrder(reloadOrder);
			}			

			std::map<std::string, std::list<Engine::Scene::Entity::Entity*>> mComponentEntities;
			const std::list<Engine::Scene::Entity::Entity*> &entities = Engine::Scene::SceneManager::getSingleton().entities();
			for (const Module *m : reloadOrder) {
				ModuleInstance &instance = mInstances.find(m)->second;

				for (const std::string &comp : instance.mEntityComponentNames) {
					std::list<Engine::Scene::Entity::Entity*> &componentEntities = mComponentEntities[comp];
					for (Engine::Scene::Entity::Entity* e : entities) {
						if (e->hasComponent(comp)) {
							componentEntities.push_back(e);
							e->removeComponent(comp);
						}
					}
				}

				qDebug() << "Unloading" << m->name();
				unloadModule(instance);
				//unload
			}
			reloadOrder.reverse();
			for (const Module *m : reloadOrder) {
				ModuleInstance &instance = mInstances.find(m)->second;

				qDebug() << "Reloading" << m->name();
				loadModule(instance, callInit);

				for (const std::string &comp : instance.mEntityComponentNames) {
					for (Engine::Scene::Entity::Entity* e : mComponentEntities[comp]) {
						e->addComponent(comp);
					}
				}

				mReloadMutex.lock();
				mInstances.find(m)->second.mNeedReload = false;
				mReloadMutex.unlock();
			}

		}

		void ModuleLoader::addModule(Module *module)
		{
			QString path = mBinaryDir + module->name() + ".dll";
			QFile file(path);

			ModuleInstance &mod = mInstances.emplace(module, module->name()).first->second;
			mod.mExists = file.exists();

			if (mod.mExists)
				mWatcher.addPath(path);			
		}


		bool ModuleLoader::unloadModule(ModuleInstance & module)
		{

			if (!module.mLoaded)
				return true;

			if (Engine::Scene::SceneManager::getSingleton().isSceneLoaded()) {
				for (Engine::Scene::SceneListener *listener : module.mSceneListeners) {
					listener->beforeSceneClear();
					listener->onSceneClear();
				}
			}

			for (Engine::Scripting::BaseGlobalAPIComponent *api : module.mGlobalAPIComponents) {
				api->finalize();
			}

			for (int i = -1; i < Engine::UI::UIManager::sMaxInitOrder; ++i)
				for (Engine::UI::GuiHandlerBase *h : module.mGuiHandlers)
					h->finalize(i);

			for (Engine::UI::GameHandlerBase *h : module.mGameHandlers) {
				h->finalize();
			}

			bool result = FreeLibrary(module.mHandle);
			if (result)
				module.mLoaded = false;
			else
				throw 0;

			return result;
		}

		bool ModuleLoader::loadModule(ModuleInstance & module, bool callInit)
		{
			if (module.mLoaded)
				return true;

			if (!module.mExists)
				return false;

			QString path = mBinaryDir + module.mName + ".dll";
			QString runtimePath = mRuntimeDir + module.mName + ".dll";
			QString pdbSource = mBinaryDir + module.mName + ".pdb";
			QString runtimePdbPath = mRuntimeDir + module.mName + ".pdb";

			qDebug() << pdbSource << " -> " << runtimePdbPath;
			qDebug() << path << " -> " << runtimePath;

			if (QFile::exists(runtimePdbPath))
				QFile::remove(runtimePdbPath);
			/*if (!*/QFile::copy(pdbSource, runtimePdbPath);//)
				//throw 0;

			while(QFile::exists(runtimePath) && !QFile::remove(runtimePath));
			while (!QFile::copy(path, runtimePath));
				

			module.mEntityComponentNames.clear();
			module.mSceneComponents.clear();
			module.mGameHandlers.clear();
			module.mGuiHandlers.clear();
			module.mGlobalAPIComponents.clear();
			std::set<std::string> beforeEntityComponents = Engine::Scene::Entity::Entity::registeredComponentNames();
			std::set<Engine::Scene::BaseSceneComponent*> beforeSceneComponents = Engine::Scene::SceneManager::getSingleton().getComponents();
			std::set<Engine::UI::GameHandlerBase*> beforeGameHandlers = Engine::UI::UIManager::getSingleton().getGameHandlers();
			std::set<Engine::UI::GuiHandlerBase*> beforeGuiHandlers = Engine::UI::UIManager::getSingleton().getGuiHandlers();
			std::set<Engine::Scripting::BaseGlobalAPIComponent*> beforeAPIComponents = Engine::Scripting::GlobalScope::getSingleton().getGlobalAPIComponents();
			std::set<Engine::Scene::SceneListener*> beforeSceneListeners = Engine::Scene::SceneManager::getSingleton().getListeners();

			UINT errorMode = GetErrorMode();
			SetErrorMode(SEM_FAILCRITICALERRORS);
			try {
				module.mHandle = LoadLibrary(runtimePath.toStdString().c_str());
			}
			catch (...) {
				module.mHandle = 0;
			}			
			SetErrorMode(errorMode);						

			if (!module.mHandle)
				return false;

			std::set<std::string> afterEntityComponents = Engine::Scene::Entity::Entity::registeredComponentNames();
			std::set_difference(afterEntityComponents.begin(), afterEntityComponents.end(), beforeEntityComponents.begin(), beforeEntityComponents.end(), std::inserter(module.mEntityComponentNames, module.mEntityComponentNames.end()));
			std::set<Engine::Scene::BaseSceneComponent*> afterSceneComponents = Engine::Scene::SceneManager::getSingleton().getComponents();
			std::set_difference(afterSceneComponents.begin(), afterSceneComponents.end(), beforeSceneComponents.begin(), beforeSceneComponents.end(), std::inserter(module.mSceneComponents, module.mSceneComponents.end()));
			std::set<Engine::UI::GameHandlerBase*> afterGameHandlers = Engine::UI::UIManager::getSingleton().getGameHandlers();
			std::set_difference(afterGameHandlers.begin(), afterGameHandlers.end(), beforeGameHandlers.begin(), beforeGameHandlers.end(), std::inserter(module.mGameHandlers, module.mGameHandlers.end()));
			std::set<Engine::UI::GuiHandlerBase*> afterGuiHandlers = Engine::UI::UIManager::getSingleton().getGuiHandlers();
			std::set_difference(afterGuiHandlers.begin(), afterGuiHandlers.end(), beforeGuiHandlers.begin(), beforeGuiHandlers.end(), std::inserter(module.mGuiHandlers, module.mGuiHandlers.end()));
			std::set<Engine::Scripting::BaseGlobalAPIComponent*> afterAPIComponents = Engine::Scripting::GlobalScope::getSingleton().getGlobalAPIComponents();
			std::set_difference(afterAPIComponents.begin(), afterAPIComponents.end(), beforeAPIComponents.begin(), beforeAPIComponents.end(), std::inserter(module.mGlobalAPIComponents, module.mGlobalAPIComponents.end()));
			std::set<Engine::Scene::SceneListener*> afterSceneListeners = Engine::Scene::SceneManager::getSingleton().getListeners();
			std::set_difference(afterSceneListeners.begin(), afterSceneListeners.end(), beforeSceneListeners.begin(), beforeSceneListeners.end(), std::inserter(module.mSceneListeners, module.mSceneListeners.end()));

			if (callInit) {

				for (int i = 0; i < Engine::UI::UIManager::sMaxInitOrder; ++i)
					for (Engine::UI::GuiHandlerBase *h : module.mGuiHandlers)
						h->init(i);

				for (Engine::UI::GameHandlerBase *h : module.mGameHandlers) {
					h->init();
				}


				for (Engine::Scene::BaseSceneComponent *c : module.mSceneComponents) {
					c->init();
				}

				for (Engine::Scripting::BaseGlobalAPIComponent *api : module.mGlobalAPIComponents) {
					api->init();
				}

				if (Engine::Scene::SceneManager::getSingleton().isSceneLoaded()) {
					for (Engine::Scene::SceneListener *listener : module.mSceneListeners) {
						listener->onSceneLoad();
					}
				}

			}

			module.mLoaded = true;

			return true;
		}

	}
}