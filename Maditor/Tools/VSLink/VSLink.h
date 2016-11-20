#pragma once

#include <maditorlib.h>

#include "../../src/Common/ProcessTalker.h"

#include <Addons\Addon.h>

#include "VSCommands.cs"

class VSLink : public QObject, public ProcessTalker, public Maditor::Addons::Addon {

	Q_OBJECT

public:
	VSLink();

	// Inherited via ProcessTalker
	virtual void receiveMessage(const Message & msg) override;

	virtual void update() override;
	virtual void addActions(Maditor::View::MainWindow *window) override;
	virtual void setModel(Maditor::Model::Editor *editor) override;

	virtual QStringList supportedFileExtensions() override;
	virtual void openFile(const QString &path, int lineNr) override;
	virtual QString resourceGroupName() override;

public slots :
	void openSolution();

private:
	void send(VSCommand cmd, const std::string &arg1 = "", int32_t arg2 = -1);
	void handle(VSCommand cmd, const std::string &arg1, int32_t arg2);

	void createInstance();

	Message mEnqueuedMsg;
	bool mMessageEnqueued;

	Maditor::Model::Editor *mEditor;
};
	
extern "C" __declspec(dllexport) Maditor::Addons::Addon *createAddon();