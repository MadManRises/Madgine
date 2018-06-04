#include "newprojectdialog.h"
#include "ui_newprojectdialog.h"

namespace Maditor {
namespace View {
namespace Dialogs {

NewProjectDialog::NewProjectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProjectDialog)
{
    ui->setupUi(this);

	connect(ui->pathEdit, &QLineEdit::textChanged, this, &NewProjectDialog::checkPath);
	connect(ui->nameEdit, &QLineEdit::textChanged, this, &NewProjectDialog::checkName);
	connect(ui->pathButton, &QPushButton::pressed, this, &NewProjectDialog::choosePath);

	ui->folderExistsLabel->setVisible(false);
	ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

NewProjectDialog::~NewProjectDialog()
{
    delete ui;
}

QString NewProjectDialog::path()
{
	return ui->pathEdit->text() + "/";
}

QString NewProjectDialog::name()
{
	return ui->nameEdit->text();
}

void NewProjectDialog::checkPath(const QString & path)
{
	bool exists = QDir(path).exists();
	if (exists) {
		checkName(name());
		ui->pathEdit->setStyleSheet("");
	}
	else {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		ui->pathEdit->setStyleSheet("color:red");
	}
}

void NewProjectDialog::checkName(const QString & name)
{
	if (name.isEmpty()) {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		ui->folderExistsLabel->setVisible(false);
	}
	else {
		bool exists = QDir(path() + name).exists();
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!exists);
		ui->folderExistsLabel->setVisible(exists);
	}
}

void NewProjectDialog::choosePath()
{
	QString p = QFileDialog::getExistingDirectory(this, "Path", path());
	if (!p.isEmpty()) {
		ui->pathEdit->setText(p);
	}
}


} // namespace Dialogs
} // namespace View
} // namespace Maditor
