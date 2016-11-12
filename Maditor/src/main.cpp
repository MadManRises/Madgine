#include "madgineinclude.h"

#include "View\mainwindow.h"
#include "Model\Editor.h"

#include "Addons\Addon.h"



//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main(int argc, char **argv) {

	int result;

	QApplication app(argc, argv);

	
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