#pragma once

#include "componentview.h"

#include "../../model/project/project.h"

#include "documents/independentwindowspawner.h"
#include "documents/windowspawner.h"

#include "propertieswidget.h"
#include "application/configwidget.h"

namespace Maditor {
namespace View {



class ProjectView : 
	public QObject, 
	public ComponentView<Model::Project>, 
	public IndependentWindowSpawner<Model::ApplicationConfig, ConfigWidget>,
	public IndependentWindowSpawner<Model::Project, PropertiesWidget>
{
    Q_OBJECT

public:

	ProjectView();
	
	virtual void setupUi(MainWindow *window) override;
	virtual void setModel(Model::Project *project) override;
	virtual void clearModel() override;

private slots:
	void openConfig(Model::ApplicationConfig *config);
	void closeConfig(Model::ApplicationConfig *config);

private:
};


} // namespace View
} // namespace Maditor

