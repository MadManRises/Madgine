

#include "VSLink.h"

#include <Windows.h>

#include <qdebug.h>

#include "View\mainwindow.h"
#include "Model\Editor.h"
#include "Model\Engine\ApplicationWrapper.h"

#include "vssettingswidget.h"

#include "View\Dialogs\settingsdialog.h"

Maditor::Addons::Addon *createAddon(Maditor::Model::Editor *editor) {
	return new VSLink(editor);
}

VSLink::VSLink(Maditor::Model::Editor *editor) :
	ProcessTalker("VSLink", "Maditor"),
	mEditor(editor),
	mSettingsWidget(0){

	editor->application()->addProcessListener([this](DWORD pid) {onProcessStarted(pid); });

	QSettings &settings = editor->settings();
	settings.beginGroup("VSLink");
	mAutoAttachDebugger = settings.value("AutoAttachDebugger").toBool();
	settings.endGroup();

}

void VSLink::openSolution() {
	send(VSCommands::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionName("debug").toStdString() );
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
		send(VSCommands::PIDAnswer, "", pid);
}

void VSLink::onProcessStarted(DWORD pid)
{
	if (mAutoAttachDebugger) {
		sendPID(pid);
	}
}

VSLink::~VSLink()
{

	QSettings &settings = mEditor->settings();
	settings.beginGroup("VSLink");
	settings.setValue("AutoAttachDebugger", mAutoAttachDebugger);
	settings.endGroup();
}

void VSLink::receiveMessage(const VSMsg & msg)
{
	handle(msg.mCmd, msg.mArg1, msg.mArg2);

}


void VSLink::addActions(Maditor::View::MainWindow * window)
{
	QToolBar *toolbar = new QToolBar;
	toolbar->setObjectName("VSLink_Toolbar");

	QList<QAction*> result;
	QAction *openAction = new QAction("OpenSolution");
	result.push_back(openAction);
	connect(openAction, &QAction::triggered, this, &VSLink::openSolution);

	toolbar->addActions(result);

	window->addToolBar(toolbar);

	mSettingsWidget = new VSSettingsWidget(this);
	Maditor::View::Dialogs::SettingsDialog *settings = window->dialogs()->settingsDialog();
	settings->addSettingsTab(mSettingsWidget, "Visual Studio");

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
				sendMsg({ VSCommands::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionName("debug").toStdString() }, "VSInstance");
			sendMsg(mEnqueuedMsg, "VSInstance");
		}
		break;
	case VSCommands::PIDRequest:
		sendPID(mEditor->application()->pid());
		break;
	default:
		qDebug() << "Unknwon Command:" << cmd << arg1.c_str() << arg2;
	}
	
}
