#pragma once

namespace Engine {

struct IndexHolder {
    virtual size_t index() const = 0;
    virtual bool isValid() const = 0;
};

}