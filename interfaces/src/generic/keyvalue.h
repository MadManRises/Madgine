#pragma once



namespace Engine
{

	enum KeyValueValueFlags
	{
		NoFlags = 0x0,
		IsEditable = 0x1
	};

	template <class T>
	struct KeyValue
	{
		static T& value(T& v)
		{
			return v;
		}

		static auto key(const T& v) -> decltype(v.key())
		{
			return v.key();
		}
	};

	template <class T>
	struct KeyValue<T*>
	{
		static T* value(T* v)
		{
			return v;
		}

		static decltype(auto) key(const T* v)
		{
			return v->key();
		}
	};

	template <class T>
	struct KeyValue<T* const>
	{
		static T* value(T* v)
		{
			return v;
		}

		static decltype(auto) key(const T* v)
		{
			return v->key();
		}
	};

	template <class K, class T>
	struct KeyValue<std::pair<K, T>>
	{
		static T& value(std::pair<K, T>& p)
		{
			return p.second;
		}

		static const K& key(const std::pair<K, T>& p)
		{
			return p.first;
		}
	};

	template <class K, class T>
	struct KeyValue<const std::pair<K, T>>
	{
		static const T& value(const std::pair<K, T>& p)
		{
			return p.second;
		}

		static const K& key(const std::pair<K, T>& p)
		{
			return p.first;
		}
	};

	template <typename T, typename D>
	struct KeyValue<std::unique_ptr<T, D>>
	{
		static Scripting::ScopeBase* value(const std::unique_ptr<T, D>& p)
		{
			return p.get();
		}

		static decltype(auto) key(const std::unique_ptr<T, D>& p)
		{
			return p->key();
		}
	};

	template <typename T, typename D>
	struct KeyValue<const std::unique_ptr<T, D>>
	{
		static Scripting::ScopeBase* value(const std::unique_ptr<T, D>& p)
		{
			return p.get();
		}

		static decltype(auto) key(const std::unique_ptr<T, D>& p)
		{
			return p->key();
		}
	};

	template <class T>
	decltype(auto) kvValue(T& v)
	{
		return KeyValue<T>::value(v);
	}

	template <class T>
	decltype(auto) kvValue(const T& v)
	{
		return KeyValue<const T>::value(v);
	}

	template <class T>
	decltype(auto) kvKey(T& v)
	{
		return KeyValue<T>::key(v);
	}

	template <class T>
	decltype(auto) kvKey(const T& v)
	{
		return KeyValue<const T>::key(v);
	}

	template <class T, class K>
	struct Finder
	{
		static auto find(T& c, const K& key)
		{
			return std::find_if(c.begin(), c.end(), [&](decltype(*c.begin()) v) { return kvKey(v) == key; });
		}
	};

	template <class V, class K, class _K>
	struct Finder<std::map<K, V>, _K>
	{
		static auto find(std::map<K, V>& c, const _K& key)
		{
			return c.find(key);
		}
	};

	template <class V, class K, class _K>
	struct Finder<const std::map<K, V>, _K>
	{
		static auto find(const std::map<K, V>& c, const _K& key)
		{
			return c.find(key);
		}
	};

	template <class T, class K>
	decltype(auto) kvFind(T& c, const K& key)
	{
		return Finder<T, K>::find(c, key);
	}

	template <class T>
	struct KeyValueFlags
	{
		static KeyValueValueFlags flags(T& t)
		{
			return NoFlags;
		}
	};

	template <class T>
	KeyValueValueFlags kvFlags(T& c)
	{
		return KeyValueFlags<T>::flags(c);
	}

	template <class T>
	KeyValueValueFlags kvFlags(const T& c)
	{
		return KeyValueFlags<const T>::flags(c);
	}

	template <class T>
	struct FixString
	{
		typedef T type;
	};

	template <>
	struct FixString<const char*>
	{
		typedef std::string type;
	};

	template <class T>
	struct KeyType
	{
		typedef typename FixString<typename std::remove_const<typename std::remove_reference<decltype(kvKey(std::declval<T>())
		)>::type>::type>::type type;
	};

}
