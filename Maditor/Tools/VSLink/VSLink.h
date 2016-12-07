#pragma once

#include <maditorlib.h>

#include "../../Common/ProcessTalker.h"

#include <Addons\Addon.h>

#include "VSCommands.cs"

#include "VSMsg.h"

class VSSettingsWidget;

class VSLink : public QObject, public ProcessTalker<VSMsg>, public Maditor::Addons::Addon {

	Q_OBJECT

public:
	VSLink(Maditor::Model::Editor *editor);
	virtual ~VSLink();

	// Inherited via ProcessTalker
	virtual void receiveMessage(const VSMsg & msg) override;

	virtual void addActions(Maditor::View::MainWindow *window) override;

	virtual QStringList supportedFileExtensions() override;
	virtual void openFile(const QString &path, int lineNr) override;
	virtual QString resourceGroupName() override;

	bool autoAttachDebugger();
	void setAutoAttachDebugger(bool b);

public slots :
	void openSolution();

private:
	void send(VSCommands::VSCommand cmd, const std::string &arg1 = "", int64_t arg2 = -1);
	void handle(VSCommands::VSCommand cmd, const std::string &arg1, int64_t arg2);

	void createInstance();

	void sendPID(DWORD pid);

	void onProcessStarted(DWORD pid);

	VSMsg mEnqueuedMsg;
	bool mMessageEnqueued;

	bool mAutoAttachDebugger;

	Maditor::Model::Editor *mEditor;

	VSSettingsWidget *mSettingsWidget;
};
	
extern "C" __declspec(dllexport) Maditor::Addons::Addon *createAddon(Maditor::Model::Editor *editor);