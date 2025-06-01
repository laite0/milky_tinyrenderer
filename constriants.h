#ifndef CONSTRIANTS_H
#define CONSTRIANTS_H

#include <concepts>

// template <typename T> concept Numeric = std::integral<T> || std::floating_point<T>;
template <typename T> concept Numeric = std::is_arithmetic_v<T>;

#endif //CONSTRIANTS_H
