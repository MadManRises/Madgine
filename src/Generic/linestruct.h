#pragma once

namespace Engine {

template <typename Tag, size_t... Ids>
struct LineStruct;

template <typename Tag, size_t Line>
struct LineStruct<Tag, Line> : LineStruct<Tag, Line - 1> {
};

template <typename Tag>
struct LineStruct<Tag, 0>;

}

#define LINE_STRUCT_EX(Tag, ...) \
    template <>                  \
    struct LineStruct<Tag, __VA_ARGS__> : LineStruct<Tag, __VA_ARGS__ - 1>
#define LINE_STRUCT(Tag) LINE_STRUCT_EX(Tag, __LINE__)
#define BASE_STRUCT(Tag) ::Engine::LineStruct<Tag, __LINE__ - 1>