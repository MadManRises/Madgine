#pragma once

#define STRINGIFY(x) #x
#define STRINGIFY2(a) STRINGIFY(a)

#define EVAL(x) x

#define CONCAT(a, b) a ## b
#define CONCAT2(a, b) CONCAT(a, b)
