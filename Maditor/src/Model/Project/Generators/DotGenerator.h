#pragma once

#include "Generator.h"

namespace Maditor {
	namespace Model {
		namespace Generators {
			class DotGenerator : public Generator {
			public:
				DotGenerator(const QString &folder, const QString &name);

				void addEdge(const QString &from, const QString &to);
				void addNode(const QString &node);

				virtual void generate() override;

			protected:
				// Inherited via Generator
				virtual QStringList filePaths() override;
				virtual void write(QTextStream & stream, int index) override;

			private:
				std::list<std::pair<QString, QString>> mEdges;
				QStringList mNodes;

				QString mFolder, mName;
				
			};
		}
	}
}