#pragma once

#include "model/treeitem.h"
#include "model/documents/document.h"

namespace Maditor {
	namespace Model {
		class MADITOR_MODEL_EXPORT ProjectElement : public TreeItem{

		public:
			ProjectElement(const QString &name, const QString &type, ProjectElement *parent);
			ProjectElement(const QString &name, const QString &type, QDomDocument &doc);
			ProjectElement(QDomElement data, ProjectElement *parent = 0);
			ProjectElement(const QString &name, ProjectElement *parent);
			ProjectElement(const ProjectElement &) = delete;
			virtual ~ProjectElement();

			const QString &name() const;
			QModelIndex ownIndex();

			virtual QString path() const;
			virtual Project *project();
			virtual ProjectElement * parentItem() const override;
			virtual QVariant cellData(int col) const override;

			bool save();
			void discardChanges();

		protected:
			QDomElement &element();
			QDomElement createElement(const QString &name);

			virtual ProjectElement *child(int i) override;

			
			void writeImpl();
			virtual bool storeData();
			virtual void restoreData();
			virtual void writeData();

			int intAttribute(const QString &name);
			int intAttribute(const QString &name, int defaultValue);
			bool boolAttribute(const QString &name);
			QString stringAttribute(const QString &name);

			void dirty();

		protected:
			const QString mName;

		private:
			QDomElement mRootElement;

			ProjectElement *mParent;

		};
	}
}
