#include "newmoduledialog.h"
#include "ui_newmoduledialog.h"
#include <qpushbutton.h>

namespace Maditor {
namespace View {
namespace Dialogs {

NewModuleDialog::NewModuleDialog(Model::Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewModuleDialog),
	mProject(project)
{
    ui->setupUi(this);

	ui->alreadyExistsMsg->setVisible(false);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

NewModuleDialog::~NewModuleDialog()
{
    delete ui;
}

QString NewModuleDialog::name()
{
	return ui->nameField->text();
}


void NewModuleDialog::nameChanged(const QString &name) {
	bool alreadyExists = !name.isEmpty() && mProject->hasModule(name);
	bool valid = !name.isEmpty() && !alreadyExists;
	ui->alreadyExistsMsg->setVisible(alreadyExists);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}


} // namespace Dialogs
} // namespace View
} // namespace Maditor
