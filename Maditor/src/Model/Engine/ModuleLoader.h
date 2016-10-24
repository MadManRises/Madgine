#pragma once

#include <qfilesystemwatcher.h>
#include <Windows.h>
#include <mutex>
#include <qset.h>
#include "forward.h"


namespace Maditor {
	namespace Model {

		class Module;
		class Project;

		class ModuleLoader : public QObject {
			Q_OBJECT

		public:
			ModuleLoader();

			void setup(const QString &binaryDir, const QString &runtimeDir, Project *project);
			
			void update(bool callInit = true);

		protected:
			void addModule(Module *module);		
			

		private slots:
			void onFileChanged(const QString &path);
			void onFolderChanged(const QString &path);

		private:
			QFileSystemWatcher mWatcher;
			QSet<QString> mFiles;

			struct ModuleInstance {
				ModuleInstance(const QString &name) :
					mHandle(0),
					mExists(false),
					mLoaded(false),
					mNeedReload(true),
					mName(name) {}
				ModuleInstance() {
					if (mLoaded)
						throw 0;
				}

				HINSTANCE mHandle;
				bool mExists;
				bool mLoaded;
				bool mNeedReload;
				QString mName;

				std::list<std::string> mEntityComponentNames;
				std::list<Engine::Scene::BaseSceneComponent*> mSceneComponents;
				std::list<Engine::UI::GameHandlerBase*> mGameHandlers;
				std::list<Engine::UI::GuiHandlerBase*> mGuiHandlers;
			};

			bool loadModule(ModuleInstance &module, bool callInit);
			bool unloadModule(ModuleInstance &module);

			std::map<const Module *, ModuleInstance> mInstances;

			QString mRuntimeDir, mBinaryDir;

			Project *mProject;

			std::mutex mReloadMutex;

			bool mNeedReload;
		};
	}
}