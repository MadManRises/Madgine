#pragma once

#include "Madgine/serialize/container/list.h"

#include "shared/moduleinstance.h"

#include "Madgine/serialize/container/action.h"

#include "tableunit.h"

#include "Madgine/signalslot/slot.h"

namespace Maditor {
	namespace Model {

		class ModuleImpl : public Shared::ModuleInstance {
		public:
			ModuleImpl(ModuleLoader *loader, const std::string &name);

		protected:
			void notify();

		private:
			ModuleLoader *mLoader;
			Engine::SignalSlot::Slot<decltype(&ModuleImpl::notify), &ModuleImpl::notify> mNotify;
		};

		class ModuleLoader : public TableUnit<ModuleLoader> {
			Q_OBJECT

		public:
			ModuleLoader(ApplicationConfig *config);
			~ModuleLoader();

			void reset();

			void setup();
			void setup2();

			bool done();

			void notifyDataChanged(ModuleImpl *module, int column);

			// Geerbt über TableUnit
			virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

			virtual QVariant data(int row, int column, int role = Qt::DisplayRole) const override;


		private slots:
			void onFileChanged(const QString &path);
			void onFolderChanged(const QString &path);

		private:
			void addModule(Module *module);

			void reload(const Module *module);

			void setupDoneImpl();

			std::tuple<ModuleLoader *, std::string> createModule(const std::string &name);

		private:
			QFileSystemWatcher mWatcher;
			QString mBinaryDir;
			QSet<QString> mFiles;

			const ModuleList &mModules;

			std::map<const Module *, Shared::ModuleInstance*> mMap;
			Engine::Serialize::ObservableList<ModuleImpl, Engine::Serialize::ContainerPolicies::allowAll, Engine::Serialize::CustomCreator<decltype(&ModuleLoader::createModule)>> mInstances;

			Engine::Serialize::Action<decltype(&ModuleLoader::setupDoneImpl), &ModuleLoader::setupDoneImpl, Engine::Serialize::ActionPolicy::request> setupDone;
			
			int mModulesCount;

			ApplicationConfig *mConfig;
			

		};
	}
}
