#pragma once

#include "Generic/container/emplace.h"

namespace Engine {

struct META_EXPORT ArgumentList {

    using value_type = ValueType;

    ArgumentList();
    ArgumentList(std::initializer_list<ValueType> vals);
    ArgumentList(size_t size);
    ArgumentList(const ArgumentList &other);
    ArgumentList(ArgumentList &&);
    ~ArgumentList();

    ArgumentList &operator=(const ArgumentList &);
    ArgumentList &operator=(ArgumentList &&);

    void clear();

    const ValueType &operator[](size_t i) const;
    ValueType &operator[](size_t i);

    void reserve(size_t size);
    void resize(size_t size);
    size_t size() const;
    void push_back(ValueType &&);
    const ValueType &at(size_t i) const;
    void insert(std::vector<ValueType>::const_iterator where, std::vector<ValueType>::const_iterator from, std::vector<ValueType>::const_iterator to);

    std::vector<ValueType>::iterator begin();
    std::vector<ValueType>::iterator end();
    std::vector<ValueType>::const_iterator begin() const;
    std::vector<ValueType>::const_iterator end() const;

private:
    std::vector<ValueType> mElements;
};

}