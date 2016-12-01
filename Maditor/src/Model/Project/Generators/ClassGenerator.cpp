#include "maditorlib.h"

#include "ClassGenerator.h"
#include "Model\Project\Module.h"
#include "HeaderGuardGenerator.h"
#include "Model\Project\Project.h"
#include "Model\Editors\EditorManager.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			ClassGenerator::ClassGenerator(Module * module, const QString & name, const QString &type) :
				ProjectElement(name, "Class", module),
				mModule(module),
				mHeaderOnly(false)
			{
				element().setAttribute("type", type);

				init();
			}
			ClassGenerator::ClassGenerator(Module * module, QDomElement data) :
				ProjectElement(data, module),
				mModule(module),
				mHeaderOnly(false)
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
				if (!mHeaderOnly)
					result << (mName + ".cpp");
				result << (mName + ".h");				
				return result;
			}

			QString ClassGenerator::path() const
			{
				return mModule->path();
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
					{ "Delete", [this]() {emit deleteClassRequest(); } }
				});
			}

			int ClassGenerator::childCount()
			{
				return 0;
			}

			TreeItem * ClassGenerator::child(int i)
			{
				throw 0;
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
					menu.addSeparator();
					QMenu *subMenu = menu.addMenu("Open");
					QObject::connect(subMenu, &QMenu::triggered, [this](QAction *action) {
						Editors::EditorManager::getSingleton().openByExtension((path() + action->text()).toStdString());
					});
					for (const QString &file : fileNames()) {
						subMenu->addAction(file);
					}
				}
			}

			void ClassGenerator::doubleClicked()
			{
				Editors::EditorManager::getSingleton().openByExtension((path() + fileNames().front()).toStdString());
			}

			Project * ClassGenerator::project()
			{
				return mModule->project();
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

		}
	}
}