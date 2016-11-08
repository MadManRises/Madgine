#include "madgineinclude.h"

#include "ModuleList.h"

#include "Project.h"

#include "Generator\DotGenerator.h"

namespace Maditor {
	namespace Model {

		
		ModuleList::ModuleList(const QString &projectRoot, Project *parent, const QString &name) :
			ProjectElement(name, "Modules", parent),
			mRoot(projectRoot + "src/"),
			mCmake(mRoot, projectRoot + "build/", name),
			mParent(parent)
		{

		}

		ModuleList::ModuleList(QDomElement element, const QString &projectRoot, Project *parent) :
			ProjectElement(element, parent),
			mRoot(projectRoot + "src/"),
			mCmake(mRoot, projectRoot + "build/", mName),
			mParent(parent)
		{
			init();

			for (QDomElement module = element.firstChildElement("Module"); !module.isNull(); module = module.nextSiblingElement("Module")) {
				addModule(new Module(this, module));
			}

			for (const std::unique_ptr<Module> &module : mModules)
				module->initDependencies();

			for (QDomElement library = element.firstChildElement("Library"); !library.isNull(); library = library.nextSiblingElement("Library")) {
				mCmake.addLibrary(library.attribute("name"));
			}
		}

		ModuleList::~ModuleList()
		{
		}

		void ModuleList::init()
		{
			setContextMenuItems({
				{ "New Module", [=]() {emit newModuleRequest(); } },
				{ "Rebuild Structure", [=]() {generate(); } },
				{ "Draw Dependencies", [=]() {drawDependenciesGraph(); } }
			});
		}

		void ModuleList::addModule(Module * module)
		{
			//beginInsertRows(index(0, 0), mModules.size(), mModules.size());
			mModules.emplace_back(module);
			//endInsertRows();

			emit moduleAdded(module);
		}

		void ModuleList::drawDependenciesGraph()
		{
			Generator::DotGenerator dot(mParent->root() + "tmp/", "Dependencies");

			for (const std::unique_ptr<Module> &module : mModules) {
				dot.addNode(module->name());
				for (const QString &dependency : module->dependencies()) {
					dot.addEdge(module->name(), dependency);
				}
			}

			dot.generate();
		}

		const QString & ModuleList::root()
		{
			return mRoot;
		}

		void ModuleList::generate()
		{
			mCmake.generate();
			mCmake.build();
		}

		void ModuleList::createModule(const QString & name)
		{
			addModule(new Module(this, name));

			generate();
			mParent->save();

		}


		Generator::CmakeProject * ModuleList::cmake()
		{
			return &mCmake;
		}

		bool ModuleList::hasModule(const QString & name)
		{
			return std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &ptr) {return ptr->name() == name; }) != mModules.end();
		}

		Module * ModuleList::getModuleByName(const QString & name)
		{
			auto it = std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &mod) {return mod->name() == name; });
			if (it == mModules.end())
				throw 0;
			return it->get();
		}

		std::list<Module*> ModuleList::initDependencies()
		{
			std::list<Module*> result;
			for (const QString &dependency : mCmake.dependencies())
				result.push_back(getModuleByName(dependency));

			return result;
		}

		std::list<std::unique_ptr<Module>>::const_iterator ModuleList::begin()
		{
			return mModules.begin();
		}

		std::list<std::unique_ptr<Module>>::const_iterator ModuleList::end()
		{
			return mModules.end();
		}

		int ModuleList::childCount() {
			return mModules.size();
		}

		Module * ModuleList::child(int i) {
			auto it = mModules.begin();
			std::advance(it, i);
			return it->get();
		}

		QVariant ModuleList::icon() const
		{
			QIcon icon;
			icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirHomeIcon),
				QIcon::Normal);
			return icon;
		}

	}
}