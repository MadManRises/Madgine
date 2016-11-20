#include "maditorlib.h"

#include "Project.h"

#include "View\Dialogs\DialogManager.h"

#include "Model\Editors\EditorManager.h"


namespace Maditor {
	namespace Model {

		const QString Project::sProjectFileName = "project.mad";

		Project::Project(const QString & path, const QString & name, QDomDocument doc) :
			ProjectElement(name, "MadProject", doc),
			TreeModel(this, 1),
			Generator(false),
			mDocument(doc),
			mPath(path + name + "/"),
			mModules(this),
			mValid(false)
		{
			init();

			QMessageBox::StandardButton answer = QMessageBox::StandardButton::Default;

			QFile file(mPath + sProjectFileName);
			if (file.exists()) {
				if (!View::Dialogs::DialogManager::confirmFileOverwriteStatic(mPath + sProjectFileName, &answer)) {
					return;
				}
			}

			copyTemplate(&answer);

			if (answer != QMessageBox::Abort) {
				mModules.generate();
				save();
				mValid = true;
			}

		}


		Project::Project(QDomDocument doc, const QString &path) :
			ProjectElement(doc.documentElement()),
			TreeModel(this, 1),
			mDocument(doc),
			mPath(path),
			mModules(element().firstChildElement("Modules"), this),
			mValid(true)
		{
			init();		

		}

		Project::~Project()
		{			
		}

		void Project::mediaDoubleClicked(const QModelIndex & index)
		{
			if (!mMediaFolder.isDir(index)) {
				Editors::EditorManager::getSingleton().openByExtension(mMediaFolder.filePath(index).toStdString());
			}
		}

		void Project::init()
		{
			mMediaFolder.setRootPath(mPath + "data/media");
		}

		void Project::copyTemplate(QMessageBox::StandardButton *answer)
		{
			QStringList templateFiles;
			QString templatePath("C:/Users/schue/Desktop/GitHub/Madgine/Maditor/templateproject/");
			QDir dir(templatePath);

			QDirIterator it(templatePath, QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
			while (it.hasNext()) {
				QString filePath = it.next();
				QString target = mPath + dir.relativeFilePath(filePath);
				QFileInfo info(filePath);
				if (info.isFile()) {
					QDir().mkpath(info.path());
					QFile targetFile(target);
					if (targetFile.exists()) {
						if (!View::Dialogs::DialogManager::confirmFileOverwriteStatic(target, answer)) {
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

		QStringList Project::filePaths()
		{
			return{ mPath + "src/main.cpp" };
		}

		void Project::write(QTextStream & stream, int index)
		{
			QString content = templateFile("main.cpp");

			stream << content;
		}

		QString Project::path() const
		{
			return mPath;
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
		
		bool Project::isValid()
		{
			return mValid;
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
			QFile file(mPath + sProjectFileName);
			file.open(QIODevice::WriteOnly | QIODevice::Truncate);
			QTextStream stream(&file);
			mDocument.save(stream, 4);
			file.close();
		}

		ModuleList * Project::moduleList()
		{
			return &mModules;
		}

		Project * Project::project()
		{
			return this;
		}

		int Project::childCount() {
			return 1;
		}

		ProjectElement * Project::child(int i) {
			switch (i) {
			case 0:
				return &mModules;
			default:
				throw 0;
			}
		}

		QFileSystemModel *Project::getMedia()
		{
			return &mMediaFolder;
		}

		void Project::release()
		{
			generate();
			mModules.release();
		}

		void Project::deleteClass(Generators::ClassGenerator *generator, bool deleteFiles) {
			if (deleteFiles)
				generator->deleteFiles();
			generator->module()->removeClass(generator);
		}


	}
}