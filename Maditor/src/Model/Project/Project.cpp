#include "Project.h"

#include <qfile.h>
#include <qapplication.h>
#include <qdir>
#include <qdiriterator>
#include <qmenu>
#include <qdebug>

#include "View\Dialogs\DialogManager.h"

#include "Generator\DotGenerator.h"

namespace Maditor {
	namespace Model {

		const QString Project::sProjectFileName = "project.mad";

		Project::Project(const QString & path, const QString & name, QDomDocument doc) :
			ProjectElement(name, "MadProject", doc),
			TreeModel(this, 1),
			mDocument(doc),
			mPath(path),
			mRoot(path + name + "/"),
			mCmake(mRoot + "src/", mRoot + "build/", name),
			mValid(false)
		{
			init();

			QMessageBox::StandardButton answer = QMessageBox::StandardButton::Default;

			QFile file(mRoot + sProjectFileName);
			if (file.exists()) {
				if (!View::Dialogs::DialogManager::confirmFileOverwrite(mRoot + sProjectFileName, &answer)) {
					return;
				}
			}

			copyTemplate(&answer);

			if (answer != QMessageBox::Abort) {
				generate();
				mValid = true;
			}

		}


		Project::Project(QDomDocument doc, const QString &path) :
			ProjectElement(doc.documentElement()),
			TreeModel(this, 1),
			mDocument(doc),
			mPath(QFileInfo(path).absolutePath()),
			mRoot(path),
			mCmake(mRoot + "src/", mRoot + "build/", mName),
			mValid(true)
		{
			init();


			for (QDomElement module = element().firstChildElement("Module"); !module.isNull(); module = module.nextSiblingElement("Module")){
				addModule(new Module(this, module));
			}

			for (const std::unique_ptr<Module> &module : mModules)
				module->initDependencies();

			for (QDomElement library = element().firstChildElement("Library"); !library.isNull(); library = library.nextSiblingElement("Library")) {
				mCmake.addLibrary(library.attribute("name"));
			}

		}

		Project::~Project()
		{
			
		}



		void Project::init()
		{
			setModelContextMenuItems({
				{ "New Module", [=]() {emit newModuleRequest(); } },
				{ "Rebuild Structure", [=]() {generate(); } },
				{ "Draw Dependencies", [=]() {drawDependenciesGraph(); }}
			});
		}

		void Project::copyTemplate(QMessageBox::StandardButton *answer)
		{
			QStringList templateFiles;
			QString templatePath("C:/Users/schue/Desktop/GitHub/Madgine/Maditor/templateproject/");
			QDir dir(templatePath);

			QDirIterator it(templatePath, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
			while (it.hasNext()) {
				QString filePath = it.next();
				QString target = mRoot + dir.relativeFilePath(filePath);
				QFileInfo info(filePath);
				if (info.isFile()) {
					QDir().mkpath(info.path());
					QFile targetFile(target);
					if (targetFile.exists()) {
						if (!View::Dialogs::DialogManager::confirmFileOverwrite(target, answer)) {
							if (*answer == QMessageBox::Abort)
								return;
							continue;
						}
						targetFile.remove();
					}
					QFile f(filePath);
					f.copy(target);
				}
				else {
					QDir().mkpath(target);
				}
			}
		}

		void Project::addModule(Module * module)
		{
			beginInsertRows(index(0,0), mModules.size(), mModules.size());
			mModules.emplace_back(module);
			endInsertRows();

			emit moduleAdded(module);
		}



		void Project::drawDependenciesGraph()
		{
			Generator::DotGenerator dot(mRoot + "tmp/", "Dependencies");

			for (const std::unique_ptr<Module> &module : mModules) {
				dot.addNode(module->name());
				for (const QString &dependency : module->dependencies()) {
					dot.addEdge(module->name(), dependency);
				}
			}

			dot.generate();
		}


		const QString & Project::root()
		{
			return mRoot;
		}

		QString Project::srcRoot()
		{
			return mRoot + "src/";
		}

		void Project::generate()
		{
			mCmake.generate();
			mCmake.build();
			save();
		}

		void Project::createModule(const QString & name)
		{
			addModule(new Module(this, name));

			generate();

		}

		Generator::CmakeProject * Project::cmake()
		{
			return &mCmake;
		}


		Project * Project::load(const QString & path)
		{
			QFile file(path + sProjectFileName);
			file.open(QIODevice::ReadOnly);
			QDomDocument doc;
			doc.setContent(&file);
			file.close();
			return new Project(doc, path);
		}


		bool Project::hasModule(const QString & name)
		{
			return std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &ptr) {return ptr->name() == name; }) != mModules.end();
		}

		bool Project::isValid()
		{
			return mValid;
		}

		const std::list<std::unique_ptr<Module>>& Project::modules()
		{
			return mModules;
		}

		Module * Project::getModuleByName(const QString & name)
		{
			auto it = std::find_if(mModules.begin(), mModules.end(), [&](const std::unique_ptr<Module> &mod) {return mod->name() == name; });
			if (it == mModules.end())
				throw 0;
			return it->get();
		}

		std::list<Module*> Project::initDependencies()
		{
			std::list<Module*> result;
			for (const QString &dependency : mCmake.dependencies())
				result.push_back(getModuleByName(dependency));

			return result;
		}


		QVariant Project::headerData(int section, Qt::Orientation orientation, int role) const
		{
			if (role != Qt::DisplayRole)
				return QVariant();
			if (orientation == Qt::Horizontal) {
				switch (section) {
				case 0:
					return "Name";
				default:
					return QVariant();
				}
			}
			else {
				return QVariant();
			}
		}

		void Project::save()
		{
			QFile file(mRoot + sProjectFileName);
			file.open(QIODevice::WriteOnly | QIODevice::Truncate);
			QTextStream stream(&file);
			mDocument.save(stream, 4);
			file.close();
		}

		int Project::childCount() {
			return mModules.size();
		}

		Module * Project::child(int i) {
			auto it = mModules.begin();
			std::advance(it, i);
			return it->get();
		}



	}
}