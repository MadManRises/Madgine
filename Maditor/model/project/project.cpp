#include "maditormodellib.h"

#include "project.h"

#include "application/applicationlauncher.h"

#include "dialogmanager.h"

#include "modulelist.h"

#include "generators/serverclassgenerator.h"

#include "configlist.h"
#include "Madgine/plugins/plugin.h"
#include "xmlexception.h"

namespace Maditor {
	namespace Model {

		const QString Project::sProjectFileName = "project.mad";

		Project::Project(LogTableModel *logs, const QDir & path, const QString & name, QDomDocument doc) :
			ProjectElement(name, "MadProject", doc),
			Document(name),
			mModel(this, 1),
			mDocument(doc),
			mPath(path),
			mModules(std::make_unique<ModuleList>(this)),
			mConfigs(std::make_unique<ConfigList>(this)),
			mValid(false),
			mLogs(logs)
		{

			QMessageBox::StandardButton answer = QMessageBox::StandardButton::Default;

			QFile file(mPath.filePath(sProjectFileName));
			if (file.exists()) {
				if (!DialogManager::confirmFileOverwriteStatic(mPath.filePath(sProjectFileName), &answer)) {
					return;
				}
			}

			copyTemplate(&answer);

			if (answer != QMessageBox::Abort) {
				init();

				mModules->generate();
				writeToDisk();
				mValid = true;
			}

		}


		Project::Project(LogTableModel *logs, QDomDocument doc, const QDir &path) :
			ProjectElement(doc.documentElement()),
			Document(doc.documentElement().attribute("name")),
			mModel(this, 1),
			mDocument(doc),
			mPath(path),
			mModules(std::make_unique<ModuleList>(uniqueChildElement("Modules"), this)),
			mConfigs(std::make_unique<ConfigList>(uniqueChildElement("Configs"), this)),
			mValid(true),
			mLogs(logs)
		{
			init();		
		}

		Project::~Project()
		{	
			Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(false);
		}

		void Project::mediaDoubleClicked(const QModelIndex & index)
		{
			if (!mMediaFolder.isDir(index)) {
				//Editors::EditorManager::getSingleton().openByExtension(mMediaFolder.filePath(index).toStdString());
			}
		}

		void Project::init()
		{
			mMediaFolder.setRootPath(mPath.filePath("data/media"));

			Engine::Serialize::Debugging::StreamDebugging::setLoggingPath(mPath.filePath("debug/runtime/maditor").toStdString());
			Engine::Serialize::Debugging::StreamDebugging::setLoggingEnabled(true);

			connect(mModules.get(), &ModuleList::classAdded, this, &Project::onClassAdded);
			connect(mConfigs.get(), &ConfigList::applicationLauncherTypeChanged, mModules.get(), &ModuleList::updateConfigs);

			for (const std::unique_ptr<ApplicationConfig> &conf : *mConfigs) {
				for (const std::unique_ptr<Module> &mod : *mModules) {
					if (conf->hasModuleEnabled(mod.get()))
						mod->addConfigs(conf->launcher(), conf->launcherType());
				}
			}

			setContextMenuItems({
				{ "Properties", [this]() {emit showProperties(); } }
			});
		}

		void Project::onClassAdded(Generators::ClassGenerator *generator) {
		}

		void Project::copyTemplate(QMessageBox::StandardButton *answer)
		{
			QStringList templateFiles;
			QString templatePath = QString::fromStdString(Engine::Plugins::Plugin::runtimePath().generic_string());
			QDir dir = QDir(templatePath).filePath("../Maditor/templateproject");

			QDirIterator it(dir.path(), QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs, QDirIterator::Subdirectories);
			while (it.hasNext()) {
				QString filePath = it.next();
				QString target = mPath.filePath(dir.relativeFilePath(filePath));
				QFileInfo info(filePath);
				if (info.isFile()) {
					QDir().mkpath(info.path());
					QFile targetFile(target);
					if (targetFile.exists()) {
						if (!DialogManager::confirmFileOverwriteStatic(target, answer)) {
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



		TreeModel * Project::model()
		{
			return &mModel;
		}

		QDir Project::path() const
		{
			return mPath;
		}

		std::unique_ptr<Project> Project::load(LogTableModel *logs, const QDir & path)
		{
			QFile file(path.filePath(sProjectFileName));
			if (!file.open(QIODevice::ReadOnly))
				throw XmlException(Engine::Database::message("Could not find Project-File '", "'!")(path.filePath(sProjectFileName).toStdString()));
			QDomDocument doc;
			doc.setContent(&file);
			file.close();
			return std::make_unique<Project>(logs, doc, path);
		}
		
		bool Project::isValid()
		{
			return mValid;
		}

		/*QVariant Project::headerData(int section, Qt::Orientation orientation, int role) const
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
		}*/

		void Project::save()
		{
			if (ProjectElement::save() && writeToDisk()) {
				writeImpl();
				Document::save();
			}
		}


		void Project::discardChanges()
		{
			ProjectElement::discardChanges();
			Document::discardChanges();
		}

		bool Project::writeToDisk()
		{
			QFile file(mPath.filePath(sProjectFileName));
			if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
				return false;
			QTextStream stream(&file);
			mDocument.save(stream, 4);
			file.close();
			return true;
		}

		LogTableModel * Project::logs()
		{
			return mLogs;
		}

		ModuleList * Project::moduleList()
		{
			return mModules.get();
		}

		ConfigList * Project::configList()
		{
			return mConfigs.get();
		}

		Project * Project::project()
		{
			return this;
		}

		int Project::childCount() const {
			return 2;
		}

		ProjectElement * Project::child(int i) {
			switch (i) {
			case 0:
				return mModules.get();
			case 1:
				return mConfigs.get();
			default:
				throw 0;
			}
		}

		QFileSystemModel *Project::getMedia()
		{
			return &mMediaFolder;
		}

		void Project::deleteClass(Generators::ClassGenerator *generator, bool deleteFiles) {
			if (deleteFiles)
				generator->deleteFiles();
			generator->module()->removeClass(generator);
		}


	}
}
