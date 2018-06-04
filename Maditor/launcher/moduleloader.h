#pragma once

#include "shared/moduleinstance.h"
#include "Madgine/serialize/container/action.h"
#include "Madgine/plugins/pluginmanager.h"

#ifdef __linux__
using HINSTANCE = void*;
#endif

namespace Maditor {

	

	namespace Launcher {

class ModuleLoader : public Engine::Serialize::SerializableUnit<ModuleLoader> {

public:
	ModuleLoader();
	~ModuleLoader();

	void setup(const std::string &binaryDir);
	void cleanup();
			
	bool receiving();

	const std::string &binaryDir();

	Engine::Plugins::PluginManager &pluginMgr();

#ifdef MADGINE_SERVER_BUILD
	Engine::Server::ServerBase * createServer(const std::string & fullName, const std::string &instanceName, const std::string &mediaDir);
#endif

private:

	class ModuleLauncherInstance : public Shared::ModuleInstance {
	public:
		ModuleLauncherInstance(ModuleLoader *parent, const std::string &name);

		~ModuleLauncherInstance();

		void createDependencies();
		bool load(bool callInit);
		bool unload();

		virtual void reloadImpl() override;

		void * getSymbol(const std::string symbolName);

	private:
		HINSTANCE mHandle;

		std::list<ModuleLauncherInstance *> mDependedBy;

		std::list<std::string> mEntityComponentNames;
		std::list<Engine::Scene::SceneComponentBase*> mSceneComponents;
		std::list<Engine::UI::GameHandlerBase*> mGameHandlers;
		std::list<Engine::UI::GuiHandlerBase*> mGuiHandlers;
		std::list<Engine::Scripting::GlobalAPIComponentBase*> mGlobalAPIComponents;
		std::list<void*> mSceneComponentHashes;
		std::list<void*> mGlobalAPIComponentHashes;
		std::list<void*> mGameHandlerHashes;
		std::list<void*> mGuiHandlerHashes;
		std::map<std::string, std::list<Engine::Scene::Entity::Entity*>> mStoredComponentEntities;

		ModuleLoader *mParent;
	};

	void setupDoneImpl();
	
private:
	std::tuple<ModuleLoader *, std::string> createModule(const std::string &name);

private:
	std::string mBinaryDir;

	bool mInit;

	bool mReceivingModules;

	Engine::Serialize::ObservableList<ModuleLauncherInstance, Engine::Serialize::ContainerPolicies::allowAll, Engine::Serialize::CustomCreator<decltype(&ModuleLoader::createModule)>> mInstances;
	Engine::Serialize::Action<decltype(&ModuleLoader::setupDoneImpl), &ModuleLoader::setupDoneImpl, Engine::Serialize::ActionPolicy::request> setupDone;

	Engine::Scripting::GlobalScopeBase *mGlobal;


	Engine::Plugins::PluginManager mPluginMgr;
};

	}
}
