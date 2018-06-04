#pragma once

#include "documents/documentview.h"
#include "model/project/applicationconfig.h"

namespace Maditor {
namespace View {

namespace Ui {
class ConfigWidget;
}

class ConfigWidget : public QWidget, public DocumentView
{
    Q_OBJECT

public:
    explicit ConfigWidget(Model::ApplicationConfig *config);
    ~ConfigWidget();

public slots:
	void setLauncher(Model::ApplicationConfig::Launcher launcher);
	void setLauncherType(Shared::LauncherType type);
	void setCustomExecutableCmd(const QString &cmd);
	void setServer(Model::Generators::ServerClassGenerator *server);

private:
    Ui::ConfigWidget *ui;

	Model::ApplicationConfig *mConfig;

};


} // namespace View
} // namespace Maditor

