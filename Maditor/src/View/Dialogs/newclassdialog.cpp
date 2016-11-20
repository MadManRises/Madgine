#include "maditorlib.h"

#include "newclassdialog.h"
#include "ui_newclassdialog.h"

#include "Model\Project\Module.h"

namespace Maditor {
namespace View {
namespace Dialogs {

NewClassDialog::NewClassDialog(Model::Module *module, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewClassDialog),
	mModule(module)
{
    ui->setupUi(this);

	ui->alreadyExistsMsg->setVisible(false);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

}

NewClassDialog::~NewClassDialog()
{
    delete ui;
}

QString NewClassDialog::name()
{
	return ui->nameField->text();
}

Model::Generators::ClassGeneratorFactory::ClassType NewClassDialog::type()
{
	return (Model::Generators::ClassGeneratorFactory::ClassType)ui->typeField->currentIndex();
}

void NewClassDialog::nameChanged(const QString &name) {
	bool alreadyExists = !name.isEmpty() && mModule->hasClass(name);
	bool valid = !name.isEmpty() && !alreadyExists;
	ui->alreadyExistsMsg->setVisible(alreadyExists);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

} // namespace Dialogs
} // namespace View
} // namespace Maditor
