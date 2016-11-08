#include "madgineinclude.h"

#include "Project.h"

#include "View\Dialogs\DialogManager.h"



namespace Maditor {
	namespace Model {

		const QString Project::sProjectFileName = "project.mad";

		Project::Project(const QString & path, const QString & name, QDomDocument doc) :
			ProjectElement(name, "MadProject", doc),
			TreeModel(this, 1),
			mDocument(doc),
			mPath(path),
			mRoot(path + name + "/"),
			mModules(mRoot, this, name),
			
			mValid(false)
		{
			init();

			QMessageBox::StandardButton answer = QMessageBox::StandardButton::Default;

			QFile file(mRoot + sProjectFileName);
			if (file.exists()) {
				if (!View::Dialogs::DialogManager::confirmFileOverwriteStatic(mRoot + sProjectFileName, &answer)) {
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
			mPath(QFileInfo(path).absolutePath()),
			mRoot(path),
			mModules(element().firstChildElement("Modules"), mRoot, this),
			mValid(true)
		{
			init();		

		}

		Project::~Project()
		{
			
		}



		void Project::init()
		{
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

		const QString & Project::root()
		{
			return mRoot;
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
			QFile file(mRoot + sProjectFileName);
			file.open(QIODevice::WriteOnly | QIODevice::Truncate);
			QTextStream stream(&file);
			mDocument.save(stream, 4);
			file.close();
		}

		ModuleList * Project::moduleList()
		{
			return &mModules;
		}

		int Project::childCount() {
			return 1;
		}

		ModuleList * Project::child(int i) {
			return &mModules;
		}



	}
}