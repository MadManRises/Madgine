#include "maditormodellib.h"

#include "modulelist.h"

#include "project.h"

#include "generators/dotgenerator.h"

#include "dialogmanager.h"

namespace Maditor {
	namespace Model {

		
		ModuleList::ModuleList(Project *parent) :
			ProjectElement("C++", "Modules", parent),
			mPath(parent->path().filePath("src")),
			mCmake(mPath, parent->path(), parent->name()),
			mParent(parent)
		{
			init();
		}

		ModuleList::ModuleList(QDomElement element, Project *parent) :
			ProjectElement(element, parent),
			mPath(parent->path().filePath("src")),
			mCmake(mPath, parent->path(), parent->name()),
			mParent(parent)
		{
			init();

			for (QDomElement module = element.firstChildElement("Module"); !module.isNull(); module = module.nextSiblingElement("Module")) {
				addModule(std::make_unique<Module>(module, this));
			}

			for (const std::unique_ptr<Module> &module : mModules)
				module->initDependencies();

			for (QDomElement library = element.firstChildElement("Library"); !library.isNull(); library = library.nextSiblingElement("Library")) {
				mCmake.project()->addLibrary(library.attribute("name"));
			}
		}

		ModuleList::~ModuleList()
		{
		}

		void ModuleList::init()
		{
			setContextMenuItems({
				{ "New Module", [=]() {newModule(); } },
				{ "Rebuild Structure", [=]() {generate(); } },
				{ "Draw Dependencies", [=]() {drawDependenciesGraph(); } }
			});
		}

		void ModuleList::addModule(std::unique_ptr<Module> &&module)
		{
			bool b = beginInsertRows(mModules.size(), mModules.size());
			Module *m = mModules.emplace_back(std::forward<std::unique_ptr<Module>>(module)).get();
			endInsertRows(b);

			connect(m, &Module::classAdded, this, &ModuleList::classAdded);

			emit moduleAdded(m);
		}

		void ModuleList::removeModule(Module* module)
		{
			emit moduleRemoved(module);

			auto it = std::find_if(mModules.begin(), mModules.end(), [=](const std::unique_ptr<Module> &mod) {return mod.get() == module; });
			size_t i = std::distance(mModules.begin(), it);
			bool b = beginRemoveRows(i, i);
			mModules.erase(it);
			endRemoveRows(b);

		}

		void ModuleList::newModule()
		{
			QString name;
			if (DialogManager::showNewModuleDialogStatic(this, name)) {
				createModule(name);
			}
		}

		void ModuleList::drawDependenciesGraph()
		{
			Generators::DotGenerator dot(mParent->path().filePath("tmp"), "Dependencies");

			for (const std::unique_ptr<Module> &module : mModules) {
				dot.addNode(module->name());
				for (const QString &dependency : module->dependencies()) {
					dot.addEdge(module->name(), dependency);
				}
			}

			dot.generate();
		}


		QDir ModuleList::path() const
		{
			return mPath;
		}

		void ModuleList::generate()
		{
			mCmake.generate();
		}

		void ModuleList::createModule(const QString & name)
		{
			addModule(std::make_unique<Module>(this, name));
			generate();
			writeData();
			mParent->writeToDisk();

		}

		void ModuleList::deleteModule(Module* module)
		{
			bool deleteFiles;
			if (DialogManager::showDeleteModuleDialogStatic(module, deleteFiles)) {
				QDir path = module->path();
				path.removeRecursively();
				removeModule(module);
				generate();
				writeData();
				mParent->writeToDisk();
			}
		}


		CmakeServer * ModuleList::cmake()
		{
			return &mCmake;
		}

		Generators::ClassGenerator * ModuleList::getClass(const QString & fullName)
		{
			QStringList l = fullName.split(":");
			if (l.size() != 2)
				return nullptr;
			return getModule(l[0])->getClass(l[1]);
		}

		bool ModuleList::hasModule(const QString & name) const
		{
			return std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &ptr) {return ptr->name() == name; }) != mModules.end();
		}

		Module * ModuleList::getModule(const QString & name)
		{
			auto it = std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &mod) {return mod->name() == name; });
			if (it == mModules.end())
				throw 0;
			return it->get();
		}

		const Module * ModuleList::getModule(const QString & name) const
		{
			auto it = std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &mod) {return mod->name() == name; });
			if (it == mModules.end())
				throw 0;
			return it->get();
		}

		std::vector<std::unique_ptr<Module>>::const_iterator ModuleList::begin() const
		{
			return mModules.begin();
		}

		std::vector<std::unique_ptr<Module>>::const_iterator ModuleList::end() const
		{
			return mModules.end();
		}

		int ModuleList::childCount() const {
			return mModules.size();
		}

		Module * ModuleList::child(int i) {
			return mModules[i].get();
		}

		void ModuleList::writeData()
		{
			generate();
		}

		QVariant ModuleList::icon() const
		{
			QIcon icon;
			icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirHomeIcon),
				QIcon::Normal);
			return icon;
		}

		void ModuleList::updateConfigs(ApplicationConfig *config, 
			ApplicationConfig::Launcher newLauncher, ApplicationConfig::Launcher oldLauncher, 
			Shared::LauncherType newLauncherType, Shared::LauncherType oldLauncherType)
		{
			for (const std::unique_ptr<Module> &mod : mModules) {
				if (config->hasModuleEnabled(mod.get())) {
					mod->updateConfigs(newLauncher, oldLauncher, newLauncherType, oldLauncherType);
				}
			}
		}



	}
}
