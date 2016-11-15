#pragma once

#include "Model\TreeItem.h"

namespace Maditor {
	namespace Model {
		class ProjectElement : public TreeItem{

		public:
			ProjectElement(const QString &name, const QString &type, ProjectElement *parent);
			ProjectElement(const QString &name, const QString &type, QDomDocument &doc);
			ProjectElement(QDomElement data, ProjectElement *parent = 0);
			virtual ~ProjectElement();

			virtual ProjectElement * parentItem() override;

			virtual QVariant data(int col) const override;

			const QString &name() const;

			virtual Project *project() = 0;

			QModelIndex ownIndex();

		protected:
			QDomDocument document();
			QDomElement element();

			int intAttribute(const QString &name);
			bool boolAttribute(const QString &name);

		protected:
			const QString mName;

		private:
			QDomElement mRootElement;

			ProjectElement *mParent;

		};
	}
}