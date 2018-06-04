#pragma once

#include "generator.h"
#include "model/project/projectelement.h"

namespace Maditor {
	namespace Model {

		namespace Generators {

			class MADITOR_MODEL_EXPORT ClassGenerator : public QObject, public ProjectElement, public Generator {
				Q_OBJECT

			public:
				ClassGenerator(Module *module, const QString &name, const QString &type, bool headerOnly = false);
				ClassGenerator(Module *module, QDomElement data);

				// Geerbt über Generator
				virtual QStringList filePaths() override;
				virtual void write(QTextStream & stream, int index) override;

				virtual QStringList fileNames();


				virtual QVariant icon() const override;

				virtual void extendContextMenu(QMenu &menu) override;

				virtual void doubleClicked() override;

				Module *module();

				void deleteFiles();

				QString fullName() const;
				QString header() const;

			signals:
				void deleteClassRequest();

			protected:
				virtual QString templateFileName(int index) = 0;
				QString templateFileByIndex(int index);

				void setKeys(std::list<std::pair<QString, QString>> &&keys);

				void init();

				bool headerOnly();

			private:
				Module *mModule;

				std::list<std::pair<QString, QString>> mModuleKeys;
				std::list<std::pair<QString, QString>> mKeys;

			};

		}
	}
}
