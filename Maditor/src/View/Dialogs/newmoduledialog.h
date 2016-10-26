#ifndef MADITOR_VIEW_DIALOGS_NEWMODULEDIALOG_H
#define MADITOR_VIEW_DIALOGS_NEWMODULEDIALOG_H


namespace Maditor {
namespace View {
namespace Dialogs {

namespace Ui {
class NewModuleDialog;
}

class NewModuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewModuleDialog(Model::Project *project, QWidget *parent = 0);
    ~NewModuleDialog();

	QString name();

public slots:
	void nameChanged(const QString &name);

private:
    Ui::NewModuleDialog *ui;

	Model::Project *mProject;
};


} // namespace Dialogs
} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_DIALOGS_NEWMODULEDIALOG_H
