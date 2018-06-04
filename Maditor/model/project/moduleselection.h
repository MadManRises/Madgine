#pragma once

#include "projectelement.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT ModuleSelection : public ProjectElement, public QAbstractListModel {
		public:
			ModuleSelection(ApplicationConfig *parent);
			ModuleSelection(QDomElement element, ApplicationConfig *parent);

			// Geerbt über QAbstractListModel
			virtual Q_INVOKABLE int rowCount(const QModelIndex & parent = QModelIndex()) const override;

			virtual Q_INVOKABLE QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

			bool storeData() override;
			void restoreData() override;

			QStringList libraries();
			QStringList files();

			bool isEnabled(const QString &name);

		public slots:
			void itemClicked(const QModelIndex &index);

		private:
			ModuleList *mModules;

			ApplicationConfig *mParent;
			
			QStringList mExcludedModules;

		};
	}
}
