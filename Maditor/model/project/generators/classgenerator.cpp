#include "maditormodellib.h"

#include "classgenerator.h"
#include "model/project/module.h"
#include "headerguardgenerator.h"
#include "model/project/project.h"
//#include "Model\Editors\EditorManager.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			ClassGenerator::ClassGenerator(Module * module, const QString & name, const QString &type, bool headerOnly) :
				ProjectElement(name, "Class", module),
				mModule(module)
			{
				element().setAttribute("type", type);
				element().setAttribute("headerOnly", headerOnly);

				init();
			}
			ClassGenerator::ClassGenerator(Module * module, QDomElement data) :
				ProjectElement(data, module),
				mModule(module)
			{
				init();
			}
			QStringList ClassGenerator::filePaths()
			{
				QStringList result;
				for (QString s : fileNames()) {
					result << (path() + s);
				}
				return result;
			}
			void ClassGenerator::write(QTextStream & stream, int index)
			{
				QString content = templateFileByIndex(index);
				
				for (const std::pair<QString, QString> &p : mModuleKeys) {
					content.replace(QString("%") + p.first, p.second);
				}

				for (const std::pair<QString, QString> &p : mKeys) {
					content.replace(QString("%") + p.first, p.second);
				}
				
				stream << content;
			}

			QStringList ClassGenerator::fileNames()
			{
				QStringList result;
				if (!headerOnly())
					result << (mName + ".cpp");
				result << (mName + ".h");				
				return result;
			}

			QString ClassGenerator::templateFileByIndex(int index)
			{
				return templateFile(templateFileName(index));
			}

			void ClassGenerator::setKeys(std::list<std::pair<QString, QString>>&& keys)
			{
				mKeys = std::forward<std::list<std::pair<QString, QString>>>(keys);
			}

			void ClassGenerator::init()
			{
				mModuleKeys = {
					{ "name", mName },
					{ "guard", HeaderGuardGenerator::guardName(mModule->name()) },
					{ "moduleHeader", HeaderGuardGenerator::fileName(mModule->name()) },
					{ "game", project()->name() },
					{ "module", mModule->name() },
					{ "header", mName + ".h" }
				};

				setContextMenuItems({
					{ "Delete", [this]() {mModule->deleteClass(this); } }
				});
			}

			bool ClassGenerator::headerOnly()
			{
				return element().hasAttribute("headerOnly") && boolAttribute("headerOnly");
			}

			QVariant ClassGenerator::icon() const
			{
				QIcon icon;
				icon.addPixmap(QApplication::style()->standardPixmap(QStyle::SP_FileIcon));
				return icon;
			}

			void ClassGenerator::extendContextMenu(QMenu & menu)
			{
				ProjectElement::extendContextMenu(menu);
				if (!fileNames().empty()){
					if (getContextMenuItems().empty() && !menu.isEmpty())
						menu.addSeparator();
					QMenu *subMenu = menu.addMenu("Open");
					QObject::connect(subMenu, &QMenu::triggered, [this](QAction *action) {
						//Editors::EditorManager::getSingleton().openByExtension((path() + action->text()).toStdString());
					});
					for (const QString &file : fileNames()) {
						subMenu->addAction(file);
					}
				}
			}

			void ClassGenerator::doubleClicked()
			{
				//Editors::EditorManager::getSingleton().openByExtension((path() + fileNames().front()).toStdString());
			}

			Module * ClassGenerator::module()
			{
				return mModule;
			}

			void ClassGenerator::deleteFiles()
			{
				for (const QString &file : filePaths()) {
					QFile(file).remove();
				}
			}

			QString ClassGenerator::fullName() const
			{
				return mModule->name() + "::" + mName;
			}

			QString ClassGenerator::header() const
			{
				return mModule->name() + "/" + mName + ".h";
			}

		}
	}
}
