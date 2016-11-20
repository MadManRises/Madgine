

#include "VSLink.h"

#include <Windows.h>

#include <qdebug.h>

#include "View\mainwindow.h"
#include "Model\Editor.h"



Maditor::Addons::Addon *createAddon() {
	return new VSLink();
}

VSLink::VSLink() :
	ProcessTalker(create, "VSLink", "Maditor") {

}

void VSLink::openSolution() {
	send(VSCommand::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionName().toStdString() );
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

void VSLink::receiveMessage(const Message & msg)
{
	handle((VSCommand)msg.mType, msg.mArg1, msg.mArg2);

}

void VSLink::update()
{
	ProcessTalker::update();
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

}

void VSLink::setModel(Maditor::Model::Editor * editor)
{
	mEditor = editor;
}

QStringList VSLink::supportedFileExtensions()
{
	return QStringList({ "cpp", "h" });
}

void VSLink::openFile(const QString & path, int lineNr)
{
	send(VSCommand::OpenSourceFile, path.toStdString(), lineNr);
}

QString VSLink::resourceGroupName()
{
	return "C++-source-file";
}


void VSLink::send(VSCommand cmd, const std::string & arg1, int32_t arg2)
{
	qDebug() << "Sending" << cmd << arg1.c_str() << arg2;

	mMessageEnqueued = false;
	mEnqueuedMsg = { cmd, arg1, arg2 };
	if (!sendMsg(mEnqueuedMsg, "VSInstance")) {
		mMessageEnqueued = true;
		createInstance();				
	}	
}

void VSLink::handle(VSCommand cmd, const std::string & arg1, int32_t arg2)
{
	switch (cmd) {
	case VSCommand::VSStarted:
		if (mMessageEnqueued) {
			mMessageEnqueued = false;
			if (mEnqueuedMsg.mType != VSCommand::OpenSolution)
				sendMsg({ VSCommand::OpenSolution, mEditor->project()->moduleList()->cmake()->solutionName().toStdString() }, "VSInstance");
			sendMsg(mEnqueuedMsg, "VSInstance");
		}
		break;
	case VSCommand::PIDRequest:
		send(VSCommand::PIDAnswer, "", GetCurrentProcessId());
		break;
	default:
		qDebug() << "Unknwon Command:" << cmd << arg1.c_str() << arg2;
	}
	
}
