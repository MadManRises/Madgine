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
			ProjectElement(name, "MadProject", doc, new TreeItem),
			mModelRootItem(parentItem()),
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
			ProjectElement(doc.documentElement(), new TreeItem),
			mModelRootItem(parentItem()),
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
			for (QMetaObject::Connection &conn : mConnections)
				QObject::disconnect(conn);
		}



		void Project::init()
		{
			mProjectIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirHomeIcon));

			mFolderIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirClosedIcon),
				QIcon::Normal, QIcon::Off);
			mFolderIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_DirOpenIcon),
				QIcon::Normal, QIcon::On);

			mFileIcon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_FileIcon));

			setContextMenuItems({
				{ "New Module", [=]() {emit newModuleRequest(); } },
				{ "Rebuild Structure", [=]() {generate(); } },
				{ "Draw Dependencies", [=]() {drawDependenciesGraph(); }}
			});
		}

		void Project::copyTemplate(QMessageBox::StandardButton *answer)
		{
			QStringList templateFiles;
			QString templatePath("C:/Users/schue/Documents/Engine---Game/Maditor/templateproject/");
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

		void Project::handleContextMenuRequest(const QModelIndex & p, QMenu & menu)
		{
			extendContextMenu(menu);
			if (p.isValid()) {
				ProjectElement *el = static_cast<ProjectElement*>(p.internalPointer());
				if (el != this)
					el->extendContextMenu(menu);
			}
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

		void Project::addView(QTreeView * view)
		{
			view->setModel(this);
			view->setContextMenuPolicy(Qt::CustomContextMenu);
			mConnections.emplace_back(connect(view, &QTreeView::customContextMenuRequested, [=](const QPoint &p) {
				QMenu menu;
				handleContextMenuRequest(view->indexAt(p), menu);
				if (menu.actions().size() > 0)
					menu.exec(view->viewport()->mapToGlobal(p));
			}));
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


		Q_INVOKABLE QModelIndex Project::index(int row, int column, const QModelIndex & parent) const
		{

			if (!hasIndex(row, column, parent))
				return QModelIndex();

			TreeItem *item;			
			if (!parent.isValid()) {
				item = mModelRootItem;
			}
			else {
				item = static_cast<TreeItem*>(parent.internalPointer());
			}

			
			if (item->childCount() > row) {
				return createIndex(row, column, item->child(row));
			}
			else {
				return QModelIndex();
			}
				
		}

		Q_INVOKABLE QModelIndex Project::parent(const QModelIndex & child) const
		{
			if (!child.isValid())
				return QModelIndex();


			TreeItem *item = static_cast<TreeItem*>(child.internalPointer());
			if (item->parentItem() == mModelRootItem)
				return QModelIndex();

			return createIndex(item->parentItem()->index(), 0, item->parentItem());
		}

		Q_INVOKABLE int Project::rowCount(const QModelIndex & parent) const
		{
			TreeItem *item;
			if (!parent.isValid())
				item = mModelRootItem;
			else {
				item = static_cast<TreeItem*>(parent.internalPointer());
			}
			return item->childCount();
		}

		Q_INVOKABLE int Project::columnCount(const QModelIndex & parent) const
		{
			return 1;
		}

		Q_INVOKABLE QVariant Project::data(const QModelIndex & index, int role) const
		{
			if (!index.isValid())
				return QVariant();

			if (role != Qt::DisplayRole && role != Qt::DecorationRole)
				return QVariant();

			ProjectElement *el = static_cast<ProjectElement*>(static_cast<TreeItem*>(index.internalPointer()));
			switch (role) {
			case Qt::DisplayRole:
				return el->name();
			case Qt::DecorationRole:
				if (el->type() == "MadProject")
					return mProjectIcon;
				else if (el->type() == "Class")
					return mFileIcon;
				else
					return mFolderIcon;
			default:
				throw 0;
			}

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

	}
}