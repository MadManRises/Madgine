#pragma once

#define STRINGIFY(x) #x
#define STRINGIFY2(a) STRINGIFY(a)

#define EVAL(x) x

#define CONCAT(a, b) a##b
#define CONCAT2(a, b) CONCAT(a, b)

#define SINGLE_ARG2(a, b) a, b
#define SINGLE_ARG3(a, b, c) a, b, c
#define SINGLE_ARG4(a, b, c, d) a, b, c, d
#define SINGLE_ARG5(a, b, c, d, e) a, b, c, d, e

#define FIRST(a, ...) a
#define SIXTH(a, b, c, d, e, f, ...) f
