#pragma once

#include "Generator.h"
#include "Model\Project\ProjectElement.h"

namespace Maditor {
	namespace Model {

		namespace Generator {

			class ClassGenerator : public Generator, public ProjectElement {

			public:
				ClassGenerator(Module *module, const QString &name, const QString &type);
				ClassGenerator(Module *module, QDomElement data);

				// Geerbt über Generator
				virtual QStringList paths() override;
				virtual void write(QTextStream & stream, int index) override;

				virtual QStringList fileNames();

				// Inherited via ProjectElement
				virtual int childCount() override;

				virtual TreeItem* child(int i) override;

				virtual QVariant icon() const override;

			protected:
				virtual QString templateFileName(int index) = 0;
				QString templateFileByIndex(int index);

				void setKeys(std::list<std::pair<QString, QString>> &&keys);

				void init();

			private:
				Module *mModule;

				bool mHeaderOnly;

				std::list<std::pair<QString, QString>> mModuleKeys;
				std::list<std::pair<QString, QString>> mKeys;


			};

		}
	}
}