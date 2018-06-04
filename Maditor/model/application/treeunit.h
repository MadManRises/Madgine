#pragma once

#include "model/treemodel.h"
#include "model/treeitem.h"

#include "Madgine/serialize/serializableunit.h"

namespace Maditor {
	namespace Model {

		class TreeUnitItemBase : public TreeItem {
		public:
			TreeUnitItemBase(TreeUnitItemBase *parent);

			virtual TreeUnitItemBase *parentItem() const override;

			QModelIndex getIndex();

		protected:
			TreeUnitItemBase(TreeUnitBase *tree);

			void notifyDataChange(int column);
			void notifyDataChange(int fromCol, int toCol);

			template <class C>
			void setContainer(C &container) {
				container.connectCallback([&container = (const C&)container, this](const typename C::const_iterator &it, int op) {
					int row = std::distance(container.begin(), it);
					handleOperation(row, op);
				});
			}

			void handleOperation(int row, int op);

		private:
			TreeUnitItemBase *mParent;
			TreeUnitBase *mTree;
		};



		class TreeUnitBase : public TreeModel, public TreeUnitItemBase {
			Q_OBJECT

		public:
			
			TreeUnitBase(int columnCount);

			virtual QVariant cellData(int column) const override;

			void handleOperation(const QModelIndex &parent, int row, int op);
			void onDataChanged(const QModelIndex &parent, int row, int fromCol, int toCol);

		private:
			bool mResetting;

		};

		template <class T>
		class TreeUnit : public TreeUnitBase, public Engine::Serialize::SerializableUnit<T> {
		public:
			TreeUnit(int columnCount) :
				TreeUnitBase(columnCount) {}
		};

		template <class T>
		class TreeUnitItem : public Engine::Serialize::SerializableUnit<T>, public TreeUnitItemBase {
		public:
			template <class U>
			TreeUnitItem(TreeUnitItem<U> *parent) :
				TreeUnitItemBase(parent) {}

		protected:
			template <class U>
			TreeUnitItem(TreeUnit<U> *tree) :
				TreeUnitItemBase(static_cast<TreeUnitItemBase*>(tree)) {}
		};

	}
}
