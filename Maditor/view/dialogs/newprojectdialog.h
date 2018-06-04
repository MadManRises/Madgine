#ifndef MADITOR_VIEW_DIALOGS_NEWPROJECTDIALOG_H
#define MADITOR_VIEW_DIALOGS_NEWPROJECTDIALOG_H

#include <QDialog>

namespace Maditor {
namespace View {
namespace Dialogs {

namespace Ui {
class NewProjectDialog;
}

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewProjectDialog(QWidget *parent = 0);
    ~NewProjectDialog();

	QString path();
	QString name();

private:
	void checkPath(const QString &path);
	void checkName(const QString &name);
	void choosePath();

private:
    Ui::NewProjectDialog *ui;
};


} // namespace Dialogs
} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_DIALOGS_NEWPROJECTDIALOG_H
