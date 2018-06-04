#include "maditorviewlib.h"

#include "applicationlog.h"
#include "ui_applicationlog.h"

#include "model/application/applicationlauncher.h"

namespace Maditor {
namespace View {

	ApplicationLog::ApplicationLog(Model::ApplicationLauncher *app) :
	DocumentView(app, this),
    ui(new Ui::ApplicationLog),
	mApp(app)
{
    ui->setupUi(this);

	setInputEnabled(app->isRunning());

	connect(app, &Model::ApplicationLauncher::outputReceived, this, &ApplicationLog::output);
	connect(app, &Model::ApplicationLauncher::applicationSetup, this, &ApplicationLog::enableInput);
	connect(app, &Model::ApplicationLauncher::applicationShutdown, this, &ApplicationLog::disableInput);
	connect(ui->clearButton, &QPushButton::clicked, this, &ApplicationLog::clear);
	if (app->isClient()) {
		ui->sendStdinButton->deleteLater();
		ui->stdinEdit->deleteLater();
	}
	else {
		connect(ui->sendStdinButton, &QPushButton::clicked, this, &ApplicationLog::sendLine);
		connect(ui->stdinEdit, &QLineEdit::returnPressed, this, &ApplicationLog::sendLine);
	}
	connect(ui->sendLuaButton, &QPushButton::clicked, this, &ApplicationLog::sendLua);
	connect(ui->luaEdit, &QLineEdit::returnPressed, this, &ApplicationLog::sendLua);

	mCursor = ui->log->textCursor();
}

	ApplicationLog::~ApplicationLog()
{
    delete ui;
}

	Model::ApplicationLauncher * ApplicationLog::app()
	{
		return mApp;
	}

	void ApplicationLog::clear()
	{
		ui->log->clear();
	}

	void ApplicationLog::sendLine()
	{
		if (mApp->isSetup())
			mApp->sendCommand(ui->stdinEdit->text());
		ui->stdinEdit->clear();
	}

	void ApplicationLog::enableInput()
	{
		setInputEnabled(true);
	}

	void ApplicationLog::disableInput()
	{
		setInputEnabled(false);
	}

	void ApplicationLog::setInputEnabled(bool b)
	{
		if (!mApp->isClient()) {
			ui->stdinEdit->setEnabled(b);
			ui->sendStdinButton->setEnabled(b);
		}
		ui->luaEdit->setEnabled(b);
		ui->sendLuaButton->setEnabled(b);
	}

	void ApplicationLog::sendLua()
	{
		if (mApp->isSetup())
			mApp->sendLua(ui->luaEdit->text());
		ui->luaEdit->clear();
	}


	void ApplicationLog::output(QString msg)
	{
		ui->log->setTextCursor(mCursor);
		msg.replace("\r\n", "\n");
		QStringList list = msg.split('\r');
		bool first = true;
		for (const QString &split : list) {
			if (first)
				first = false;
			else {
				ui->log->moveCursor(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
			}			
			if (!split.isEmpty())
				ui->log->insertPlainText(split);
		}
		mCursor = ui->log->textCursor();
		ui->log->ensureCursorVisible();
	}


} // namespace View
} // namespace Maditor
