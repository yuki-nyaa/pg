#pragma once
#include<concepts>
#include<type_traits>
#include<yuki/pg/core.hpp>

namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t,typename T,typename Loc=Location_Point>
struct SToken{
    Token_Kind_t kind_ = -1;
    T value;
    Loc location={};

    constexpr Token_Kind_t kind() const {return kind_;}
};

template<std::unsigned_integral Token_Kind_t,typename Loc>
struct SToken<Token_Kind_t,void,Loc>{
    Token_Kind_t kind_ = -1;
    Loc location={};

    constexpr Token_Kind_t kind() const {return kind_;}
};

template<std::unsigned_integral Token_Kind_t,typename T>
struct SToken<Token_Kind_t,T,No_Location>{
    Token_Kind_t kind_ = -1;
    T value;

    constexpr Token_Kind_t kind() const {return kind_;}
};

template<std::unsigned_integral Token_Kind_t>
struct SToken<Token_Kind_t,void,No_Location>{
    Token_Kind_t kind_ = -1;

    constexpr Token_Kind_t kind() const {return kind_;}
};

static_assert(std::is_aggregate_v<SToken<unsigned,Location_Point,Location_Range>>);
} // namespace yuki::pg
