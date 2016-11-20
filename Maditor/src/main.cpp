#include "madgineinclude.h"

#include "View\mainwindow.h"
#include "Model\Editor.h"

#include "Addons\Addon.h"

#include "Common\Shared.h"


SharedMemory *SharedMemory::msSingleton = 0;

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main(int argc, char **argv) {

	SharedMemory mem(create);

	int result;

	QApplication app(argc, argv);

#ifdef MADITOR_STATIC
	Q_INIT_RESOURCE(resources);
#endif
	
	{
		Maditor::Addons::AddonCollector addons;

		Maditor::Model::Editor editor(&addons);

		addons.setModel(&editor);
		{
			Maditor::View::MainWindow w(&editor);

			addons.setWindow(&w);

			w.startup();

			w.show();

			result = app.exec();
		}
	}


	return result; 
}