
#include "Generator.h"
#include <qfile.h>
#include <QMessageBox>
#include "View\Dialogs\DialogManager.h"

namespace Maditor {
	namespace Model {
		namespace Generator {

			Generator::Generator(bool askOverride) :
				mAskOverride(askOverride)
			{
			}

			void Generator::generate()
			{
				QMessageBox::StandardButton answer = QMessageBox::Default;


				int i = 0;
				for (QString path : paths()) {
					QFile file(path);
					if (mAskOverride && file.exists()) {
						if (!View::Dialogs::DialogManager::confirmFileOverwrite(path, &answer)) {
							if (answer == QMessageBox::Abort)
								return;
							continue;
						}
					}
					if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
					{
						throw 0;
					}
					QTextStream stream(&file);

					write(stream, i);

					stream.flush();
					file.flush();
					file.close();

					++i;
				}
				
			}

			QString Generator::templateFile(const QString & name)
			{
				QFile file(QString("C:/Users/schue/Desktop/GitHub/Madgine/Maditor/templates/") + name);
				if (!file.open(QFile::ReadOnly | QFile::Text)) {
					throw 0;
				}
				QTextStream stream(&file);
				QString result = stream.readAll();
				file.close();
				return result;
			}


		}
	}
}