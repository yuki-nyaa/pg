#pragma once
#include<concepts>
#include<type_traits>
#include<yuki/pg/core.hpp>

namespace yuki::pg{

template<std::unsigned_integral Token_Kind_t,typename T>
struct SToken{
    Token_Kind_t kind_ = -1;
    T value;
    Location_Range location_range={};

    constexpr Token_Kind_t kind() const {return kind_;}
};

template<std::unsigned_integral Token_Kind_t>
struct SToken<Token_Kind_t,void>{
    Token_Kind_t kind_ = -1;
    Location_Range location_range={};

    constexpr Token_Kind_t kind() const {return kind_;}
};

static_assert(std::is_aggregate_v<SToken<unsigned,Location_Range>>);
} // namespace yuki::pg