#include "maditormodellib.h"
#include "scopewrapper.h"

#include "inspector.h"

namespace Maditor {
	namespace Model {

		ValueItem::ValueItem(ScopeWrapperItem * parent, const std::string & name, const Engine::ValueType & value, const std::string &type, Engine::KeyValueValueFlags flags) :
			mParent(parent),
			mName(name),
			mValue(value),
			mFlags(flags),
			mType(type.empty() ? value.getTypeString() : type)
		{

		}


		void ValueItem::setValue(const Engine::ValueType & val)
		{
			mValue = val;
		}

		void ValueItem::requestSetValue(const Engine::ValueType &val)
		{
			mValue = val;
			mParent->setField(mName, val);
		}

		Engine::KeyValueValueFlags ValueItem::flags()
		{
			return mFlags;
		}

		const std::string& ValueItem::name() const
		{
			return mName;
		}


		const Engine::ValueType& ValueItem::value() const
		{
			return mValue;
		}

		QString ValueItem::type()
		{
			return QString::fromStdString(mType);
		}

		ScopeWrapperItem::ScopeWrapperItem(Inspector * parent, Engine::InvScopePtr ptr) :	
			mInspector(parent),
			mPtr(ptr)
		{
		}

		ScopeWrapperItem::~ScopeWrapperItem()
		{
		}


		void ScopeWrapperItem::clear()
		{
			mValues.clear();
		}

		void ScopeWrapperItem::update(const std::string &key, const std::map<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>>& data)
		{			
			mKey = QString::fromStdString(key);
			std::set<std::string> attributes;
			for (const std::pair<const std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &p : data) {
				std::pair<std::map<std::string, ValueItem>::iterator, bool> result = mValues.try_emplace(p.first, this, p.first, std::get<0>(p.second), std::get<1>(p.second), std::get<2>(p.second));
				int row = std::distance(mValues.begin(), result.first);
				if (!result.second) {
					result.first->second.setValue(std::get<0>(p.second));
				}
				attributes.insert(p.first);
			}
			int i = 0;
			for (auto it = mValues.begin(); it != mValues.end(); ) {
				if (attributes.find(it->first) == attributes.end()) {				
					it = mValues.erase(it);					
				}
				else {
					++it;
					++i;
				}
			}
		}

		std::map<std::string, ValueItem>& ScopeWrapperItem::values()
		{
			return mValues;
		}

		void ScopeWrapperItem::setType(const QString &type)
		{
			mType = type;
		}

		const QString& ScopeWrapperItem::type() const
		{
			return mType;
		}

		const QString& ScopeWrapperItem::key() const
		{
			return mKey;
		}

		Engine::InvScopePtr ScopeWrapperItem::ptr()
		{
			return mPtr;
		}

		ScopeWrapperItem* ScopeWrapperItem::resolve(Engine::InvScopePtr ptr)
		{
			return mInspector->getScope(ptr);
		}

		void ScopeWrapperItem::setField(const std::string& name, const Engine::ValueType& value)
		{
			mInspector->setField(mPtr, name, value);
		}

	}
}
