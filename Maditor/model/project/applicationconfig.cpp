#include "maditormodellib.h"

#include "applicationconfig.h"

#include "configlist.h"

#include "application/standalonelauncher.h"
#include "application/embeddedlauncher.h"

#include "project.h"

#include "../shared/applicationinfo.h"

#include "modulelist.h"

#include "generators/serverclassgenerator.h"

#include "generators/commandline.h"

#include "Madgine/serialize/container/noparent.h"

namespace Maditor
{
	namespace Model
	{
		ApplicationConfig::ApplicationConfig(ConfigList* parent, const QString& name) :
			ProjectElement(name, "Config", parent),
			Document(name),
			Generator(false),
			mParent(parent),
			mLauncher(MADITOR_LAUNCHER),
			mLauncherType(Shared::CLIENT_LAUNCHER),
			mServer(nullptr),
			mInstanceCounter(0),
			mModules(this)
		{
			storeData();
			init();
		}

		ApplicationConfig::ApplicationConfig(QDomElement data, ConfigList* parent) :
			ProjectElement(data, parent),
			Document(data.attribute("name")),
			Generator(false),
			mParent(parent),
			mInstanceCounter(0),
			mModules(data.firstChildElement("ModuleSelection"), this)
		{
			restoreData();
			init();
		}

		ApplicationConfig::~ApplicationConfig()
		{
		}

		void ApplicationConfig::generateInfo(Shared::ApplicationInfo& appInfo, QWindow* w) const
		{
			appInfo.mDataDir = (mParent->path() + "Data/").toStdString().c_str();
			appInfo.mProjectDir = mParent->path().toStdString().c_str();
			appInfo.mType = mLauncherType;

			if (mLauncherType == Shared::CLIENT_LAUNCHER)
			{
				if (w){
					appInfo.mWindowHandle = static_cast<size_t>(w->winId());
					appInfo.mWindowWidth = w->width();
					appInfo.mWindowHeight = w->height();
				}

				appInfo.mServerClass.clear();
			}
			else
			{
				appInfo.mServerClass = mServer->fullName().toStdString().c_str();
			}
		}

		bool ApplicationConfig::hasModuleEnabled(Module* module)
		{
			return mModules.isEnabled(module->name());
		}

		ApplicationLauncher* ApplicationConfig::createInstace(bool remote)
		{
			if (!remote)
			{
				return mDocuments.createDocument<Engine::Serialize::NoParentUnit<EmbeddedLauncher>>(
					this, name() + "-" + QString::number(++mInstanceCounter));
			}
			else
			{
				return mDocuments.createDocument<Engine::Serialize::NoParentUnit<StandaloneLauncher>>(
					this, name() + "-" + QString::number(++mInstanceCounter));
			}
		}

		ApplicationConfig::Launcher ApplicationConfig::launcher() const
		{
			return mLauncher;
		}

		const QString& ApplicationConfig::customExecutableCmd() const
		{
			return mCustomExecutableCmd;
		}

		Shared::LauncherType ApplicationConfig::launcherType() const
		{
			return mLauncherType;
		}

		Generators::ServerClassGenerator* ApplicationConfig::server() const
		{
			return mServer;
		}

		QString ApplicationConfig::path() const
		{
			return mParent->path() + "release/" + name() + "/";
		}

		void ApplicationConfig::save()
		{
			if (ProjectElement::save() && project()->writeToDisk())
				Document::save();
		}

		void ApplicationConfig::discardChanges()
		{
			ProjectElement::discardChanges();
			Document::discardChanges();
		}

		ModuleSelection* ApplicationConfig::modules()
		{
			return &mModules;
		}

		void ApplicationConfig::release()
		{
			generate();
			std::string cmd = QString("\"cmake -G \"" CMAKE_GENERATOR "\" -B\"%1\" -H\"%2\"\"").arg(
				path() + "build/", path() + "src/").toStdString();
			Generators::CommandLine::exec(cmd.c_str());
		}

		void ApplicationConfig::setLauncherType(Shared::LauncherType type)
		{
			if (mLauncherType != type)
			{
				emit launcherTypeChanged(type, mLauncherType);
				mLauncherType = type;
				setDirtyFlag(true);
			}
		}

		void ApplicationConfig::setCustomExecutableCmd(const QString& cmd)
		{
			mCustomExecutableCmd = cmd;
			setDirtyFlag(true);
			emit customExecutableCmdChanged(cmd);
		}

		void ApplicationConfig::setServer(Generators::ServerClassGenerator* server)
		{
			mServer = server;
			setDirtyFlag(true);
			emit serverChanged(server);
		}

		void ApplicationConfig::setServerByName(const QString& name)
		{
			setServer(project()->moduleList()->getClass<Generators::ServerClassGenerator>(name));
		}

		void ApplicationConfig::setLauncher(Launcher launcher)
		{
			if (mLauncher != launcher)
			{
				emit launcherChanged(launcher, mLauncher);
				mLauncher = launcher;
				setDirtyFlag(true);
				if (launcher == MADITOR_LAUNCHER)
				{
					setContextMenuItems({
						{"Release", [this]() { release(); }}
					});
				}
				else
				{
					setContextMenuItems({});
				}
			}
		}

		void ApplicationConfig::doubleClicked()
		{
			emit mParent->openConfig(this);
		}

		bool ApplicationConfig::storeData()
		{
			element().setAttribute("launcher", mLauncher);
			element().setAttribute("launcherType", mLauncherType);
			element().setAttribute("customExecutableCmd", mCustomExecutableCmd);
			element().setAttribute("server", mServer ? mServer->fullName() : "");
			mModules.storeData();
			return true;
		}

		void ApplicationConfig::restoreData()
		{
			setLauncher((Launcher)intAttribute("launcher", MADITOR_LAUNCHER));
			setLauncherType((Shared::LauncherType)intAttribute("launcherType", Shared::CLIENT_LAUNCHER));
			setCustomExecutableCmd(stringAttribute("customExecutableCmd"));
			setServerByName(stringAttribute("server"));
			mModules.restoreData();
			setDirtyFlag(false);
		}

		void ApplicationConfig::init()
		{
			connect(&mDocuments, &DocumentStore::documentCreated, this, &ApplicationConfig::onDocumentCreated);
			connect(&mDocuments, &DocumentStore::documentDestroyed, this, &ApplicationConfig::onDocumentDestroyed);
		}

		void ApplicationConfig::onDocumentCreated(Document* doc)
		{
			emit documentCreated(static_cast<ApplicationLauncher*>(doc));
		}

		void ApplicationConfig::onDocumentDestroyed(Document* doc)
		{
			emit documentDestroyed(static_cast<ApplicationLauncher*>(doc));
		}

		QStringList ApplicationConfig::filePaths()
		{
			return {
				path() + "src/CmakeLists.txt",
				path() + "src/main.cpp"
			};
		}

		void ApplicationConfig::write(QTextStream& stream, int index)
		{
			assert(mLauncher == MADITOR_LAUNCHER);

			QString content;

			if (index == 0)
			{
				QString libraries = mModules.libraries().join(" ");

				QString files = mModules.files().join("\n");

				QString type = (mLauncherType == Shared::SERVER_LAUNCHER ? "TRUE" : "FALSE");

				content = templateFile("CmakeRelease.txt").arg(name(), libraries, files, project()->moduleList()->path(), type);
			}
			else if (index == 1)
			{
				if (mLauncherType == Shared::SERVER_LAUNCHER)
				{
					content = templateFile("ServerMain.cpp").arg(mServer->header(), mServer->fullName(), project()->name(), name());
				}
				else
				{
					content = templateFile("ClientMain.cpp");
				}
			}
			else
			{
				throw 0;
			}

			stream << content;
		}
	}
}
