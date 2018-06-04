#include <maditorviewlib.h>

#include "VSLink.h"

#include <Windows.h>
#undef NO_ERROR

#include <qdebug.h>

#include "View\mainwindow.h"
#include "View\Dialogs\DialogManager.h"
#include "Model\Maditor.h"
#include "Model\Application\EmbeddedLauncher.h"

#include "Model\Project\ModuleList.h"

#include "VSLinkView.h"

#include "Shared\ApplicationInfo.h"

#include "Model\Project\ConfigList.h"


Maditor::Addons::Addon *createAddon(Maditor::Model::Maditor *editor) {
	return new VSLink(editor);
}

VSLink::VSLink(Maditor::Model::Maditor *editor) :
	ProcessTalker("VSLink", "Maditor"),
	mEditor(editor),
	mView(new VSLinkView(this))
{
	QSettings &settings = editor->settings();
	settings.beginGroup("VSLink");
	mAutoAttachDebugger = settings.value("AutoAttachDebugger").toBool();
	settings.endGroup();
}


VSLink::~VSLink()
{
	QSettings &settings = mEditor->settings();
	settings.beginGroup("VSLink");
	settings.setValue("AutoAttachDebugger", mAutoAttachDebugger);
	settings.endGroup();
}

void VSLink::openSolution() {
	send(VSCommands::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionPath().toStdString() );
}


void VSLink::createInstance()
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	// set the size of the structures
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	CreateProcess(NULL,   // the path
		CMAKE_VS_DEVENV_COMMAND " /command \"Tools.EnablePlugin\"",
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi           // Pointer to PROCESS_INFORMATION structure
	);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

}

void VSLink::sendPID(DWORD pid)
{
	if (pid)
		sendMsg({ VSCommands::PIDAnswer, "", pid }, "VSInstance");
}

void VSLink::onProcessStarted(DWORD pid, const Maditor::Shared::ApplicationInfo &info)
{
	if (mAutoAttachDebugger && info.mDebugged) {
		sendPID(pid);
	}
}

void VSLink::receiveMessage(const VSMsg & msg)
{
	handle(msg.mCmd, msg.mArg1, msg.mArg2);

}

void VSLink::setup(Maditor::View::MaditorView * view)
{
	mView->setup(view);
}

void VSLink::setupUi(Maditor::View::MainWindow * window)
{
	mView->setupUi(window);

}

QStringList VSLink::supportedFileExtensions()
{
	return QStringList({ "cpp", "h" });
}

void VSLink::openFile(const QString & path, int lineNr)
{
	send(VSCommands::OpenSourceFile, path.toStdString(), lineNr);
}

QString VSLink::resourceGroupName()
{
	return "C++-source-file";
}

void VSLink::onProjectOpened(Maditor::Model::Project * project)
{
	connect(project->configList(), &Maditor::Model::ConfigList::instanceAdded, this, &VSLink::onInstanceAdded);
}

void VSLink::onInstanceAdded(Maditor::Model::Document * app)
{
	if (Maditor::Model::EmbeddedLauncher *launcher = dynamic_cast<Maditor::Model::EmbeddedLauncher*>(app))
		connect(launcher, &Maditor::Model::EmbeddedLauncher::processStarted, this, &VSLink::onProcessStarted);
}

bool VSLink::autoAttachDebugger()
{
	return mAutoAttachDebugger;
}

void VSLink::setAutoAttachDebugger(bool b)
{
	mAutoAttachDebugger = b;
}


void VSLink::send(VSCommands::VSCommand cmd, const std::string & arg1, int64_t arg2)
{
	qDebug() << "Sending" << cmd << arg1.c_str() << arg2;

	mMessageEnqueued = false;
	mEnqueuedMsg = { cmd, arg1, arg2 };
	
	if (!sendMsg(mEnqueuedMsg, "VSInstance")) {
		mMessageEnqueued = true;
		createInstance();				
	}	
}

void VSLink::handle(VSCommands::VSCommand cmd, const std::string & arg1, int64_t arg2)
{
	switch (cmd) {
	case VSCommands::VSStarted:
		if (mMessageEnqueued) {
			mMessageEnqueued = false;
			if (mEnqueuedMsg.mCmd != VSCommands::OpenSolution)
				sendMsg({ VSCommands::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionPath().toStdString() }, "VSInstance");
			sendMsg(mEnqueuedMsg, "VSInstance");
		}
		break;
	/*case VSCommands::PIDRequest:
		sendPID(mEditor->project()->application()->pid());
		break;*/
	default:
		qDebug() << "Unknwon Command:" << cmd << arg1.c_str() << arg2;
	}
	
}
