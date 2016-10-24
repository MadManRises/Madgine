#include <qapplication.h>
#include "View\mainwindow.h"
#include "Model\Editor.h"

#include <libinclude.h>
#include "Util\UtilMethods.h"

#include "Scripting\Datatypes\argumentlist.h"
#include "Scripting\Datatypes\valuetype.h"



Q_DECLARE_METATYPE(Ogre::LogMessageLevel);
Q_DECLARE_METATYPE(QList<Engine::Util::TraceBack>);

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main(int argc, char **argv) {


	int result;


	Maditor::Model::Editor editor;
	{
		QApplication app(argc, argv);
		qRegisterMetaType<Ogre::LogMessageLevel>();
		qRegisterMetaType<QList<Engine::Util::TraceBack>>();

		{


			Maditor::View::MainWindow w(&editor);
			w.show();

			result = app.exec();

		}
	}
	return result; 
}