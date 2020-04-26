#pragma once

namespace Engine {

template <typename Tag, size_t Line>
struct LineStruct : LineStruct<Tag, Line - 1> {
};

template <typename Tag>
struct LineStruct<Tag, 0>;

}