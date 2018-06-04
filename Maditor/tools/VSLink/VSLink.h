#pragma once

#include "../ProcessTalker.h"

#include <Addons\Addon.h>

#include "VSCommands.cs"

#include "VSMsg.h"

#include <Windows.h>
#undef NO_ERROR

class VSLinkView;

class VSLink : public QObject, public ProcessTalker<VSMsg>, public Maditor::Addons::Addon {

	Q_OBJECT

public:
	VSLink(Maditor::Model::Maditor *editor);
	virtual ~VSLink();

	// Inherited via ProcessTalker
	virtual void receiveMessage(const VSMsg & msg) override;

	virtual void setup(Maditor::View::MaditorView *view) override;
	virtual void setupUi(Maditor::View::MainWindow *window) override;

	virtual QStringList supportedFileExtensions() override;
	virtual void openFile(const QString &path, int lineNr) override;
	virtual QString resourceGroupName() override;

	virtual void onProjectOpened(Maditor::Model::Project *project) override;
	void onInstanceAdded(Maditor::Model::Document *app);

	bool autoAttachDebugger();
	void setAutoAttachDebugger(bool b);

public slots :
	void openSolution();

private:
	void send(VSCommands::VSCommand cmd, const std::string &arg1 = "", int64_t arg2 = -1);
	void handle(VSCommands::VSCommand cmd, const std::string &arg1, int64_t arg2);

	void createInstance();

	void sendPID(DWORD pid);

	void onProcessStarted(DWORD pid, const Maditor::Shared::ApplicationInfo &info);

	VSMsg mEnqueuedMsg;
	bool mMessageEnqueued;

	bool mAutoAttachDebugger;

	Maditor::Model::Maditor *mEditor;

	VSLinkView *mView;

};
	
extern "C" __declspec(dllexport) Maditor::Addons::Addon *createAddon(Maditor::Model::Maditor *editor);