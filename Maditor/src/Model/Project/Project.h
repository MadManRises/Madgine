#pragma once

#include "Generator\CmakeProject.h"
#include <qdom.h>
#include "Module.h"
#include "ProjectElement.h"
#include <qMessageBox>

#include <qtreeview.h>

namespace Maditor {
	namespace Model {
		class Project : public QAbstractItemModel, public ProjectElement {
			Q_OBJECT

		public:
			Project(const QString &path, const QString &name, QDomDocument doc = QDomDocument());
			Project(QDomDocument doc, const QString &path);
			~Project();

			const QString &root();
			QString srcRoot();

			void generate();

			Generator::CmakeProject *cmake();

			static Project *load(const QString &path);

			bool hasModule(const QString &name);

			bool isValid();

			const std::list<std::unique_ptr<Module>> &modules();
			Module *getModuleByName(const QString &name);
			std::list<Module *> initDependencies();

			// Geerbt über QAbstractItemModel
			virtual Q_INVOKABLE QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const override;
			virtual Q_INVOKABLE QModelIndex parent(const QModelIndex & child) const override;
			virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;
			virtual Q_INVOKABLE int columnCount(const QModelIndex & parent = QModelIndex()) const override;
			virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

			virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

			void handleContextMenuRequest(const QModelIndex &p, QMenu &menu);

		protected:
			void save();

		private:
			void init();

			void copyTemplate(QMessageBox::StandardButton *answer);

			void addModule(Module *module);

		public slots:
			void createModule(const QString &name);
			void drawDependenciesGraph();

		signals:
			void newModuleRequest();
			void moduleAdded(Module *);

		private:
			QDomDocument mDocument;
			
			QString mPath, mRoot;

			Generator::CmakeProject mCmake;

			std::list<std::unique_ptr<Module>> mModules;

			QIcon mProjectIcon;
			QIcon mFolderIcon;
			QIcon mFileIcon;

			TreeItem *mModelRootItem;

			bool mValid;

			static const QString sProjectFileName;


		};
	}
}