#include "maditorviewlib.h"

#include "configwidget.h"
#include "ui_configwidget.h"

#include "model/project/applicationconfig.h"

#include "model/project/project.h"
#include "model/project/modulelist.h"

#include "model/project/generators/serverclassgenerator.h"

namespace Maditor {
namespace View {

ConfigWidget::ConfigWidget(Model::ApplicationConfig *config) :
	DocumentView(config, this),
    ui(new Ui::ConfigWidget),
	mConfig(config)
{
    ui->setupUi(this);

	ui->modulesView->setModel(config->modules());
	
	ui->launcherGroup->setId(ui->maditorLauncherButton, Model::ApplicationConfig::MADITOR_LAUNCHER);
	ui->launcherGroup->setId(ui->customLauncherButton, Model::ApplicationConfig::CUSTOM_LAUNCHER);

	ui->launcherTypeGroup->setId(ui->clientButton, Shared::CLIENT_LAUNCHER);
	ui->launcherTypeGroup->setId(ui->serverButton, Shared::SERVER_LAUNCHER);

	setLauncher(config->launcher());
	setLauncherType(config->launcherType());
	setCustomExecutableCmd(config->customExecutableCmd());

	std::list<Model::Generators::ServerClassGenerator*> serverClasses = config->project()->moduleList()->getClasses<Model::Generators::ServerClassGenerator>();
	if (!serverClasses.empty()) {
		for (Model::Generators::ServerClassGenerator *server : serverClasses) {
			ui->serverName->addItem(server->fullName());
		}
		if (!config->server())
			config->setServer(serverClasses.front());
		setServer(config->server());
		ui->serverButton->setEnabled(true);
	}
	else {
		ui->serverButton->setEnabled(false);
	}	

	connect(ui->launcherGroup, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), [=](int id, bool checked) {if (checked) mConfig->setLauncher((Model::ApplicationConfig::Launcher)id); });
	connect(ui->launcherTypeGroup, static_cast<void(QButtonGroup::*)(int, bool)>(&QButtonGroup::buttonToggled), [=](int id, bool checked) {if (checked) mConfig->setLauncherType((Shared::LauncherType)id); });
	connect(ui->customLauncherCmd, &QLineEdit::textChanged, config, &Model::ApplicationConfig::setCustomExecutableCmd);
	connect(ui->serverName, &QComboBox::currentTextChanged, config, &Model::ApplicationConfig::setServerByName);
	connect(ui->modulesView, &QListView::clicked, config->modules(), &Model::ModuleSelection::itemClicked);
	connect(config, &Model::ApplicationConfig::launcherChanged, this, &ConfigWidget::setLauncher);
}

ConfigWidget::~ConfigWidget()
{
    delete ui;
}

void ConfigWidget::setLauncher(Model::ApplicationConfig::Launcher type)
{
	ui->launcherGroup->button(type)->setChecked(true);
}

void ConfigWidget::setLauncherType(Shared::LauncherType type)
{
	ui->launcherTypeGroup->button(type)->setChecked(true);
}

void ConfigWidget::setCustomExecutableCmd(const QString & cmd)
{
	ui->customLauncherCmd->setText(cmd);
}

void ConfigWidget::setServer(Model::Generators::ServerClassGenerator * server)
{
	ui->serverName->setCurrentText(server->fullName());
}


} // namespace View
} // namespace Maditor
