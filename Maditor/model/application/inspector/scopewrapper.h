#pragma once

#include "../treeunit.h"

namespace Maditor {
	namespace Model {

		class MADITOR_MODEL_EXPORT ValueItem {
		public:
			ValueItem(ScopeWrapperItem *parent, const std::string &name, const Engine::ValueType &value, const std::string &type, Engine::KeyValueValueFlags flags);

			void setValue(const Engine::ValueType &val);
			void requestSetValue(const Engine::ValueType &val);
			Engine::KeyValueValueFlags flags();

			const std::string &name() const;
			const Engine::ValueType &value() const;

			QString type();

		private:
			std::string mName;
			Engine::ValueType mValue;
			std::string mType;
			ScopeWrapperItem *mParent;
			Engine::KeyValueValueFlags mFlags;
		};


		class MADITOR_MODEL_EXPORT ScopeWrapperItem {
		public:
			ScopeWrapperItem(Inspector *parent, Engine::InvScopePtr ptr);
			virtual ~ScopeWrapperItem();

			void clear();

			void update(const std::string &key, const std::map<std::string, std::tuple<Engine::ValueType, std::string, Engine::KeyValueValueFlags>> &data);

			std::map<std::string, ValueItem> &values();

			void setType(const QString &tag);
			const QString &type() const;

			const QString &key() const;

			Engine::InvScopePtr ptr();

			ScopeWrapperItem *resolve(Engine::InvScopePtr ptr);

			void setField(const std::string& name, const Engine::ValueType& value);
			

		private:

			std::map<std::string, ValueItem> mValues;

			Engine::InvScopePtr mPtr;
			
			Inspector *mInspector;

			QString mType;

			QString mKey;

		};
	}
}
