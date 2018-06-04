#pragma once

#include "documents/documentview.h"

namespace Maditor {
namespace View {

namespace Ui {
class ApplicationLog;
}

class ApplicationLog : public QWidget, public DocumentView
{
    Q_OBJECT

public:
    explicit ApplicationLog(Model::ApplicationLauncher *app);
    ~ApplicationLog();

	Model::ApplicationLauncher *app();

private slots:
	void output(QString msg);
	void clear();
	void sendLine();

	void enableInput();
	void disableInput();
	void setInputEnabled(bool b);

	void sendLua();

private:
    Ui::ApplicationLog *ui;

	Model::ApplicationLauncher *mApp;

	QTextCursor mCursor;

};


} // namespace View
} // namespace Maditor

