#include "modulepropertieswidget.h"
#include "modulepropertieswidget.h"
#include "ui_modulepropertieswidget.h"

#include "model/project/modulelist.h"

namespace Maditor {
namespace View {

ModulePropertiesWidget::ModulePropertiesWidget(Model::Module *module) :
    QWidget(),
	mModule(module),
    ui(new Ui::ModulePropertiesWidget)
{
    ui->setupUi(this);

	for (const std::unique_ptr<Model::Module> &dep : *module->parent()) {
		QListWidgetItem *item = new QListWidgetItem(dep->name(), ui->dependenciesList);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setCheckState(module->dependencies().contains(dep->name()) ? Qt::Checked : Qt::Unchecked);
	}

	connect(ui->dependenciesList, &QListWidget::itemChanged, this, &ModulePropertiesWidget::update);

	ui->dependenciesList->sortItems();

}

ModulePropertiesWidget::~ModulePropertiesWidget()
{
    delete ui;
}

void ModulePropertiesWidget::update(QListWidgetItem * item)
{
	bool isChecked = item->checkState() == Qt::Checked;
	bool isDep = mModule->dependencies().contains(item->text());
	if (isChecked != isDep) {
		if (!isChecked) {
			mModule->removeDependency(item->text());
		}
		else {
			if (!mModule->addDependency(item->text())) {
				QMessageBox::critical(0, "Cannot set Dependencies!",
					QString("Dependency to '%1' can not be set to Module '%2'").arg(item->text(), mModule->name()));
				item->setCheckState(Qt::Unchecked);
			}
		}
	}
}

} // namespace View
} // namespace Maditor
