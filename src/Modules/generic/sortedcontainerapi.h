#pragma once

namespace Engine {
template <typename C>
struct SortedContainerApi : C {

    using C::C;
    using C::operator=;

    using traits = typename C::traits;

    typedef typename traits::iterator iterator;
    typedef typename traits::const_iterator const_iterator;

    typedef typename traits::key_type key_type;
    typedef typename traits::value_type value_type;

    /*iterator find(const key_type& key)
			{
				return kvFind(this->mData, key);
			}*/

    bool contains(const key_type &key)
    {
        return this->find(key) != this->end();
    }

    template <typename... _Ty>
    std::pair<iterator, bool> emplace(_Ty &&... args)
    {
        return this->C::emplace(this->end(), std::forward<_Ty>(args)...);
    }

    /*template <class Ty, class _ = decltype(std::declval<typename C::NativeContainerType>().find(std::declval<Ty>()))>
			iterator find(const Ty& v)
			{
				return this->mData.find(v);
			}*/
};
}
