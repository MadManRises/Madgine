#include "maditormodellib.h"
#include "scopewrapper.h"

#include "inspector.h"

namespace Maditor {
	namespace Model {

		ValueItem::ValueItem(ScopeWrapperItem * parent, const std::string & name, const Engine::ValueType & value) :
			mParent(parent),
			mName(name),
			mValue(value),
			mEditable(true)
		{

		}

		QVariant ValueItem::cellData(int col) const
		{
			QVariant result;
			switch (col) {
			case 0:
				result = QString::fromStdString(mName);
				break;
			case 1:
				result.setValue(mValue);
				break;
			}
			return result;
		}

		TreeItem * ValueItem::parentItem() const
		{
			return mParent;
		}

		void ValueItem::setValue(const Engine::ValueType & val)
		{
			mValue = val;
		}

		bool ValueItem::isEditable()
		{
			return mEditable;
		}

		void ValueItem::setEditable(bool b) {
			mEditable = b;
		}

		ScopeWrapperItem::ScopeWrapperItem(Inspector * parent, Engine::InvScopePtr ptr) :	
			mInspector(parent),
			mPtr(ptr)
		{
		}

		ScopeWrapperItem::~ScopeWrapperItem()
		{
		}

		int ScopeWrapperItem::childCount() const
		{
			return mValues.size();
		}

		void ScopeWrapperItem::clear()
		{
			mValues.clear();
		}

		TreeItem * ScopeWrapperItem::child(int i)
		{
			auto it = mValues.begin();
			std::advance(it, i);
			return &it->second;
		}

		QVariant ScopeWrapperItem::cellData(int col) const
		{
			QVariant result;
			switch (col) {
			case 0:
				break;
			case 1:
				result.setValue(Engine::ValueType(mPtr));
				break;
			}
			return result;
		}

		void ScopeWrapperItem::update(const std::map<std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>>& data)
		{			
			std::set<std::string> attributes;
			for (const std::pair<const std::string, std::tuple<Engine::ValueType, Engine::KeyValueValueFlags>> &p : data) {
				std::pair<std::map<std::string, ValueItem>::iterator, bool> result = mValues.try_emplace(p.first, this, p.first, std::get<0>(p.second));
				result.first->second.setEditable(std::get<1>(p.second) & Engine::KeyValueValueFlags::IsEditable);
				int row = std::distance(mValues.begin(), result.first);
				if (!result.second) {
					result.first->second.setValue(std::get<0>(p.second));
					mInspector->dataChanged(mInspector->index(row, 1, getIndex()), mInspector->index(row, 1, getIndex()));
				}
				else {
					mInspector->beginInsertRows(getIndex(), row, row);
					mInspector->endInsertRows();
				}
				attributes.insert(p.first);
			}
			int i = 0;
			for (auto it = mValues.begin(); it != mValues.end(); ) {
				if (attributes.find(it->first) == attributes.end()) {
					mInspector->beginRemoveRows(getIndex(), i, i);
					it = mValues.erase(it);
					mInspector->endRemoveRows();
				}
				else {
					++it;
					++i;
				}
			}
		}

		QModelIndex ScopeWrapperItem::getIndex() {
			return mInspector->index(parentIndex(), 0);
		}

		TreeItem *ScopeWrapperItem::parentItem() const {
			return mInspector;
		}


	}
}
