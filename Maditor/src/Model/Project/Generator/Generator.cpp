#include "madgineinclude.h"

#include "Generator.h"
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
				for (QString path : filePaths()) {
					try {
						QFile file(path);
						if (mAskOverride && file.exists()) {
							if (!View::Dialogs::DialogManager::confirmFileOverwriteStatic(path, &answer)) {
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
					}
					catch (const std::exception &e) {
						QMessageBox::critical(0, "Error", e.what());
					}
					++i;
				}
				
			}

			QString Generator::templateFile(const QString & name)
			{
				QString path = QString("C:/Users/schue/Desktop/GitHub/Madgine/Maditor/templates/") + name;
				QFile file(path);
				if (!file.open(QFile::ReadOnly | QFile::Text)) {
					throw std::exception((std::string("Could not open File: ") + path.toStdString()).c_str());
				}
				QTextStream stream(&file);
				QString result = stream.readAll();
				file.close();
				return result;
			}


		}
	}
}