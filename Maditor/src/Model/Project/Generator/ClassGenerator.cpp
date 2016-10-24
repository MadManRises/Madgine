#include "ClassGenerator.h"
#include "Model\Project\Module.h"
#include "Model\TreeItem.h"
#include <QDebug>
#include "HeaderGuardGenerator.h"
#include "Model\Project\Project.h"
#include <qapplication.h>

namespace Maditor {
	namespace Model {
		namespace Generator {
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
			QStringList ClassGenerator::paths()
			{
				QStringList result;
				for (QString s : fileNames()) {
					result << (mModule->root() + s);
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
				result << (mName + ".h");
				if (!mHeaderOnly)
					result << (mName + ".cpp");
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
					{ "game", mModule->parent()->name() },
					{ "module", mModule->name() },
					{ "header", mName + ".h" }
				};
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


		}
	}
}