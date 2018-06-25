#pragma once

#include "componentview.h"

#include "../../model/project/cmakeserver.h"

namespace Maditor {
namespace View {



class CmakeView : 
	public QObject, 
	public ComponentView<Model::CmakeServer>
{
    Q_OBJECT

public:

	CmakeView();
	
	virtual void setupUi(MainWindow *window) override;
	virtual void setModel(Model::CmakeServer *server) override;
	virtual void clearModel() override;

private slots:

private:

	QStatusBar *mStatusBar;
	QProgressBar *mProgress;
};


} // namespace View
} // namespace Maditor

