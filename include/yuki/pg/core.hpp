#pragma once
#include<cstddef>
#include<yuki/Vector.hpp>

namespace yuki::pg{

struct Location_Point{
    size_t line=0;
    size_t column=0;
};

struct Location_Range{
    Location_Point begin={};
    Location_Point end={};
};

struct No_Location {};

enum struct Token_Impl_Type : unsigned char {VARIANT,SIMPLE,TUPLE};

template<typename T>
struct Token_State_Pair{
    T token;
    size_t state;
};

template<typename T>
using stack_t = yuki::Vector<T>;
} // namespace yuki::pg
