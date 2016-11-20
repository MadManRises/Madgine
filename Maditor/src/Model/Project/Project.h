#pragma once

#include "Generators\Generator.h"

#include "ProjectElement.h"

#include "Model\TreeModel.h"
#include "ModuleList.h"

namespace Maditor {
	namespace Model {
		class MADITOR_EXPORT Project : public TreeModel, public ProjectElement, public Generators::Generator {
			Q_OBJECT

		public:
			Project(const QString &path, const QString &name, QDomDocument doc = QDomDocument());
			Project(QDomDocument doc, const QString &path);
			~Project();

			virtual QString path() const override;

			static Project *load(const QString &path);

			bool isValid();

			virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
					
			void save();

			ModuleList *moduleList();

			virtual Project *project() override;

			// Inherited via ProjectElement
			virtual int childCount() override;

			virtual ProjectElement* child(int i) override;

			QFileSystemModel *getMedia();

			void release();

			// Geerbt über Generator
			virtual QStringList filePaths() override;

			virtual void write(QTextStream & stream, int index) override;

		public slots:
			void deleteClass(Generators::ClassGenerator *generator, bool deleteFiles);

			void mediaDoubleClicked(const QModelIndex &index);

		private:
			void init();

			void copyTemplate(QMessageBox::StandardButton *answer);

			

		
		private:
			QDomDocument mDocument;
			
			QString mPath;

			bool mValid;

			static const QString sProjectFileName;

			ModuleList mModules;

			QFileSystemModel mMediaFolder;




		};
	}
}