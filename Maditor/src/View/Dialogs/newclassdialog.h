#ifndef MADITOR_VIEW_DIALOGS_NEWCLASSDIALOG_H
#define MADITOR_VIEW_DIALOGS_NEWCLASSDIALOG_H

#include "Model/Project\Generators/ClassGeneratorFactory.h"

namespace Maditor {
namespace View {
namespace Dialogs {

namespace Ui {
class NewClassDialog;
}

class NewClassDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewClassDialog(Model::Module *module, QWidget *parent = 0);
    ~NewClassDialog();

	QString name();
	Model::Generators::ClassGeneratorFactory::ClassType type();

public slots:
	void nameChanged(const QString &name);


private:
    Ui::NewClassDialog *ui;

	Model::Module *mModule;
};


} // namespace Dialogs
} // namespace View
} // namespace Maditor
#endif // MADITOR_VIEW_DIALOGS_NEWCLASSDIALOG_H
