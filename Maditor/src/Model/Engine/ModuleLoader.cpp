#include "maditorinclude.h"

#include "ModuleLoader.h"

#include "Model\Project\Project.h"


namespace Maditor {
	namespace Model {
		typedef void (*voidF)();

		ModuleLoader::ModuleLoader() :
			mProject(0),
			mNeedReload(false)
		{
			connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &ModuleLoader::onFileChanged);
			connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, &ModuleLoader::onFolderChanged);
		}

		void ModuleLoader::onFileChanged(const QString &path) {
			QFileInfo f(path);
			ModuleInstance &module = mInstances.find(mProject->getModuleByName(f.baseName()))->second;
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
				if (mProject->hasModule(f.baseName())) {
					ModuleInstance &module = mInstances.find(mProject->getModuleByName(f.baseName()))->second;
					mReloadMutex.lock();
					module.mExists = true;
					module.mNeedReload = true;
					mNeedReload = true;
					mReloadMutex.unlock();
				}
			}

		}

		void ModuleLoader::setup(const QString & binaryDir, const QString & runtimeDir, Project *project)
		{
			mRuntimeDir = runtimeDir;
			mBinaryDir = binaryDir;
			mProject = project;

			QDir dir(binaryDir);
			mFiles = QSet<QString>::fromList(dir.entryList({"*.dll"}, QDir::NoDotAndDotDot | QDir::Files));

			SetDllDirectory(runtimeDir.toStdString().c_str());

			mWatcher.addPath(binaryDir);

			
			mNeedReload = true;

			mInstances.clear();
			
			for (const std::unique_ptr<Module> &module : project->modules()) {
				addModule(module.get());
			}

			update(false);

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

			bool isSceneLoaded = Engine::Scene::SceneManager::getSingleton().isSceneLoaded();

			for (Engine::Scene::BaseSceneComponent *c : module.mSceneComponents) {
				if (isSceneLoaded)
					c->onSceneClear();
			}

			for (Engine::UI::GameHandlerBase *h : module.mGameHandlers) {
				if (isSceneLoaded)
					h->onSceneClear();
			}


			bool result = FreeLibrary(module.mHandle);
			if (result)
				module.mLoaded = false;

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
			std::set<std::string> beforeEntityComponents = Engine::Scene::Entity::Entity::registeredComponentNames();
			std::set<Engine::Scene::BaseSceneComponent*> beforeSceneComponents = Engine::Scene::SceneManager::getSingleton().getComponents();
			std::set<Engine::UI::GameHandlerBase*> beforeGameHandlers = Engine::UI::UIManager::getSingleton().getGameHandlers();
			std::set<Engine::UI::GuiHandlerBase*> beforeGuiHandlers = Engine::UI::UIManager::getSingleton().getGuiHandlers();

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

			for (const std::string &t : module.mEntityComponentNames) {
				qDebug() << t.c_str();
			}

			

			if (callInit) {
				bool isSceneLoaded = Engine::Scene::SceneManager::getSingleton().isSceneLoaded();

				for (int i = 0; i < Engine::UI::UIManager::sMaxInitOrder; ++i)
					for (Engine::UI::GuiHandlerBase *h : module.mGuiHandlers)
						h->init(i);

				for (Engine::UI::GameHandlerBase *h : module.mGameHandlers) {
					h->init();
					if (isSceneLoaded)
						h->onSceneLoad();
				}


				for (Engine::Scene::BaseSceneComponent *c : module.mSceneComponents) {
					c->init();
					if (isSceneLoaded)
						c->onSceneLoad();
				}
			}

			module.mLoaded = true;

			return true;
		}

	}
}