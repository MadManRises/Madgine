#include "maditorviewlib.h"

#include "newmoduledialog.h"
#include "ui_newmoduledialog.h"

#include "model/project/project.h"
#include "model/project/modulelist.h"

namespace Maditor {
namespace View {
namespace Dialogs {

NewModuleDialog::NewModuleDialog(Model::ModuleList *list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewModuleDialog),
	mList(list)
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
	bool alreadyExists = !name.isEmpty() && mList->hasModule(name);
	bool valid = !name.isEmpty() && !alreadyExists;
	ui->alreadyExistsMsg->setVisible(alreadyExists);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}


} // namespace Dialogs
} // namespace View
} // namespace Maditor
