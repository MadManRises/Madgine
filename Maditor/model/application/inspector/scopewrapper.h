#pragma once

#include "model/application/treeunit.h"

namespace Maditor {
	namespace Model {

		class ValueItem : public TreeItem {
		public:
			ValueItem(ScopeWrapperItem *parent, const std::string &name, const Engine::ValueType &value);

			virtual TreeItem *child(int) override { throw 0; };
			virtual QVariant cellData(int col) const override;
			virtual TreeItem *parentItem() const override;

			void setValue(const Engine::ValueType &val);
			bool isEditable();
			void setEditable(bool b);

		private:
			std::string mName;
			Engine::ValueType mValue;
			ScopeWrapperItem *mParent;
			bool mEditable;
		};


		class ScopeWrapperItem : public TreeItem {
		public:
			ScopeWrapperItem(Inspector *parent, Engine::InvScopePtr ptr);
			virtual ~ScopeWrapperItem();

			void clear();

			virtual int childCount() const override;
			virtual TreeItem *child(int i) override;
			virtual QVariant cellData(int col) const override;		
			virtual TreeItem *parentItem() const override;

			void update(const std::map<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &data);

			QModelIndex getIndex();

		private:

			std::map<std::string, ValueItem> mValues;

			Engine::InvScopePtr mPtr;
			
			Inspector *mInspector;

		};
	}
}
