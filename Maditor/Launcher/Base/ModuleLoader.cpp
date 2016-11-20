
#include "libinclude.h"

#include "ModuleLoader.h"


		ModuleLoader::ModuleLoader() :
			ProcessTalker("Maditor_Module_Loader", "Loader"),
			mInit(false)
		{
			
		}


		ModuleLoader::~ModuleLoader()
		{
		}

		void ModuleLoader::setup(const std::string &binaryDir, const std::string & runtimeDir)
		{
			
			assert(!mInit);
			mInit = true;

			mRuntimeDir = runtimeDir;
			mBinaryDir = binaryDir;

			mReceivingModules = true;

			sendMsg({ Init }, "Maditor_Loader");
	
			while (mReceivingModules) {
				update();
			}
		}

		void ModuleLoader::cleanup()
		{

			for (std::pair<const std::string, ModuleInstance> &p : mInstances) {
				unloadModule(p.second);
			}

			mInstances.clear();
			
			mInit = false;
		}

		

		ModuleLoader::ModuleInstance &ModuleLoader::addModule(const std::string &name)
		{
			auto it = mInstances.find(name);
			if (it == mInstances.end()) {
				return mInstances.emplace(name, name).first->second;
			}
			else {
				return it->second;
			}
	
		}


		bool ModuleLoader::unloadModule(ModuleInstance & module)
		{

			if (!module.mLoaded)
				return true;

			std::cout << "Unloading " << module.mName << std::endl;
			
			if (Engine::Scene::SceneManager::getSingleton().isSceneLoaded()) {

				const std::list<Engine::Scene::Entity::Entity*> &entities = Engine::Scene::SceneManager::getSingleton().entities();


				for (const std::string &comp : module.mEntityComponentNames) {
					std::list<Engine::Scene::Entity::Entity*> &componentEntities = module.mStoredComponentEntities[comp];
					for (Engine::Scene::Entity::Entity* e : entities) {
						if (e->hasComponent(comp)) {
							componentEntities.push_back(e);
							e->removeComponent(comp);
						}
					}
				}


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


		void ModuleLoader::receiveMessage(const ModuleLoaderMsg & msg)
		{
			switch (msg.mCmd) {
			case Init:
				mReceivingModules = false;
				break;
			case Load:
				loadModule(addModule(msg.mArg), msg.mCallInit);
				break;
			case Unload:
				unloadModule(addModule(msg.mArg));
				break;
			}
		}

		bool ModuleLoader::loadModule(ModuleInstance & module, bool callInit)
		{
			if (module.mLoaded)
				return true;

			std::cout << "Loading " << module.mName << std::endl;


			std::string runtimePath = mRuntimeDir + module.mName + ".dll";
			std::string runtimePdbPath = mRuntimeDir + module.mName + ".pdb";
			std::string binaryPath = mBinaryDir + module.mName + ".dll";
			std::string binaryPdbPath = mBinaryDir + module.mName + ".dll";

			{
				std::ifstream  src(binaryPath, std::ios::binary);
				std::ofstream  dst(runtimePath, std::ios::binary);

				dst << src.rdbuf();
			}

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
				module.mHandle = LoadLibrary(runtimePath.c_str());
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

					for (const std::pair<const std::string, std::list<Engine::Scene::Entity::Entity*>> &ents : module.mStoredComponentEntities) {
						for (Engine::Scene::Entity::Entity* e : ents.second) {
							e->addComponent(ents.first);
						}
					}
					module.mStoredComponentEntities.clear();
				}

			}

			module.mLoaded = true;

			return true;
		}


