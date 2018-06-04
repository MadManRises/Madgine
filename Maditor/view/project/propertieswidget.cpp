#include "maditorviewlib.h"

#include "propertieswidget.h"
#include "ui_propertieswidget.h"
#include "model/project/module.h"
#include "model/project/project.h"
#include "model/project/modulelist.h"
#include "project/modulepropertieswidget.h"

namespace Maditor {
	namespace View {

			PropertiesWidget::PropertiesWidget(Model::Project *project) :
				mProject(project),
				ui(new Ui::PropertiesWidget),
				DocumentView(project, this)
			{
				ui->setupUi(this);

				//ui->propertiesWidget->setRowCount(list->childCount());

				//int i = 0;

				//setWindowTitle(windowTitle().arg(module->name()));
				for (const std::unique_ptr<Model::Module> &module : *mProject->moduleList()) {
					
					ModulePropertiesWidget *widget = new ModulePropertiesWidget(module.get());
					ui->dependenciesWidget->addTab(widget, module->name());
					mModuleWidgets.push_back(widget);
				
					

					
					/*ui->propertiesWidget->setItem(i, 0, new QTableWidgetItem(module->name()));

					QTableWidgetItem *clientItem = new QTableWidgetItem;

					clientItem->setFlags(clientItem->flags() | Qt::ItemIsUserCheckable);
					clientItem->setCheckState(module->clientCode() ? Qt::Checked : Qt::Unchecked);
					ui->propertiesWidget->setItem(i, 1, clientItem);

					QTableWidgetItem *serverItem = new QTableWidgetItem;

					serverItem->setFlags(serverItem->flags() | Qt::ItemIsUserCheckable);
					serverItem->setCheckState(module->serverCode() ? Qt::Checked : Qt::Unchecked);
					ui->propertiesWidget->setItem(i, 2, serverItem);

					++i;*/
				}

				//connect(ui->buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &PropertiesWidget::apply);
				//connect(ui->propertiesWidget, &QTableView::clicked, list, &Model::ModuleList::onItemClicked);
				

			}

			PropertiesWidget::~PropertiesWidget()
			{
				delete ui;
			}

			/*bool PropertiesWidget::apply()
			{
				bool valid = true;

				for (ModulePropertiesWidget *widget : mModuleWidgets) {
					valid &= widget->apply();
				}*/

				/*int i = 0;
				for (const std::unique_ptr<Model::Module> &module : *mModules) {
					if (!module->setClientCode(ui->propertiesWidget->item(i, 1)->checkState() == Qt::Checked))
						QMessageBox::critical(0, "Cannot update ClientCode-Property!",
							QString("ClientCode can not be set to %1 for Module '%2'").arg(module->clientCode() ? "true" : "false", module->name()));
					if (!module->setServerCode(ui->propertiesWidget->item(i, 2)->checkState() == Qt::Checked))
						QMessageBox::critical(0, "Cannot update ServerCode-Property!",
							QString("ServerCode can not be set to %1 for Module '%2'").arg(module->clientCode() ? "true" : "false", module->name()));
					++i;
				}*/

				

			/*	if (valid) {
					mModules->project()->save();
					mModules->generate();
				}

				return valid;
			}*/

	
	}
}
