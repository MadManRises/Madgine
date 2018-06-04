#include "maditormodellib.h"

#include "moduleloader.h"

#include "model/project/project.h"

#include "model/project/module.h"

#include "model/project/modulelist.h"

#include "applicationlauncher.h"

#include "project/applicationconfig.h"

namespace Maditor {
	namespace Model {


		ModuleLoader::ModuleLoader(ApplicationConfig *config) :			
			mBinaryDir(config->project()->path() + "debug/bin/"),
			mModules(*config->project()->moduleList()),
			TableUnit(2),
			mModulesCount(-1),
			mConfig(config)
		{
			setContainer(mInstances);
						
			mInstances.setCreator(std::bind(&ModuleLoader::createModule, this, std::placeholders::_1));

			QDir dir(mBinaryDir);
			mFiles = QSet<QString>::fromList(dir.entryList({ "*.dll" }, QDir::NoDotAndDotDot | QDir::Files));


			connect(&mWatcher, &QFileSystemWatcher::fileChanged, this, &ModuleLoader::onFileChanged);
			connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, &ModuleLoader::onFolderChanged);

			mWatcher.addPath(mBinaryDir);
			qDebug() << mBinaryDir;
		}


		ModuleLoader::~ModuleLoader()
		{
			if (!mBinaryDir.isEmpty())
				mWatcher.removePath(mBinaryDir);

			std::list<const Module*> reloadOrder;

			for (const Shared::ModuleInstance &mod : mInstances) {
				mWatcher.removePath(mBinaryDir + QString::fromStdString(mod.name()) + ".dll");
			}

			mInstances.clear();

		}

		void ModuleLoader::reset()
		{
			mInstances.clear();
		}

		void ModuleLoader::onFileChanged(const QString &path) {

			QFileInfo f(path);
			const Module *m = mModules.getModule(f.baseName());
			auto it = mMap.find(m);
			//if (it == mInstances.end())
			//	return;
			Shared::ModuleInstance &module = *it->second;
			module.setExists(f.exists());
			if (module.exists()) {
				qDebug() << path << "Changed!";
				reload(m);
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
				if (mModules.hasModule(f.baseName())) {
					reload(mModules.getModule(f.baseName()));
				}
			}

		}


		

		void ModuleLoader::addModule(Module *module)
		{
			
			auto it = mInstances.emplace_back(this, module->name().toStdString());



			/*[=](ModuleImpl &item) {
				QString path = mBinaryDir + module->name() + ".dll";
				QFile file(path);

				mMap[module] = &item;
				item.setExists(file.exists());

				if (file.exists())
					mWatcher.addPath(path);

			}*/
		}


		void ModuleLoader::setup()
		{
			mModulesCount = 0;
			for (const std::unique_ptr<Module> &module : mModules) {
				if (mConfig->hasModuleEnabled(module.get()) && !module->empty()) {
					addModule(module.get());
					++mModulesCount;
				}
			}

		}

		void ModuleLoader::setup2()
		{
			for (Shared::ModuleInstance &module : mInstances) {
				const Module *mod = mModules.getModule(QString::fromStdString(module.name()));
				mMap[mod] = &module;
				for (const QString &dep : mod->dependencies()) {
					std::string dependency = dep.toStdString();
					module.addDependency(&*std::find_if(mInstances.begin(), mInstances.end(), [&](const Shared::ModuleInstance &m) {return m.name() == dependency; }));
				}
			}
			setupDone({});
		}

		bool ModuleLoader::done()
		{
			return mInstances.size() == mModulesCount;
		}

		void ModuleLoader::notifyDataChanged(ModuleImpl * module, int column)
		{
			int row = std::distance(mInstances.begin(), std::find_if(mInstances.begin(), mInstances.end(), [=](const ModuleImpl &mod) { return &mod == module; }));
			QModelIndex i = index(row, column);
			emit dataChanged(i, i);
		}

		void ModuleLoader::reload(const Module *module) {

			mMap.at(module)->reload({});
		}

		void ModuleLoader::setupDoneImpl()
		{
		}

		Q_INVOKABLE int ModuleLoader::rowCount(const QModelIndex & parent) const
		{
			return Q_INVOKABLE mInstances.size();
		}

		QVariant ModuleLoader::data(int row, int column, int role) const
		{
			auto it = mInstances.begin();
			std::advance(it, row);
			switch (role) {
			case Qt::DisplayRole:
				switch (column) {
				case 0:
					return QString::fromStdString(it->name());
				case 1:
					return it->isLoaded();
				}
				break;
			}
			return Q_INVOKABLE QVariant();
		}

		std::tuple<ModuleLoader*, std::string> ModuleLoader::createModule(const std::string & name)
		{
			return{ this, name };
		}

		ModuleImpl::ModuleImpl(ModuleLoader * loader, const std::string & name) :
			ModuleInstance(name),
			mLoader(loader),
			mNotify(this)
		{
			mLoaded.setCallback(mNotify);
		}

		void ModuleImpl::notify()
		{
			mLoader->notifyDataChanged(this, 1);
		}

}
}
