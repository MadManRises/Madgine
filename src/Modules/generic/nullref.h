#pragma once

namespace Engine {

template <typename T>
T &nullref = *reinterpret_cast<T *>(0x1);

}