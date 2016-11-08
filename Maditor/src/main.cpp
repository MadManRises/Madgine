#include "madgineinclude.h"

#include "View\mainwindow.h"
#include "Model\Editor.h"

#include "Addons\Addon.h"



//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main(int argc, char **argv) {

	int result;


	Maditor::Model::Editor editor;
	{
		QApplication app(argc, argv);
		{
			Maditor::View::MainWindow w(&editor);

			Maditor::Addons::AddonCollector addons(&w, &editor);

			w.show();

			result = app.exec();
		}
	}
	return result; 
}