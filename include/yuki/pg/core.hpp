#pragma once
#include<cstddef>

namespace yuki::pg{
struct Location_Point{
    size_t line=0;
    size_t column=0;
};
struct Location_Range{
    Location_Point begin={};
    Location_Point end={};
};

enum struct Token_Impl_Type : unsigned char {VARIANT,SIMPLE,ANY};

template<typename TS>
struct AbsParser{
    typedef TS Token_Settings;

    typedef typename TS::Token_Kind Token_Kind;
    typedef typename TS::Token Token;

    typedef typename TS::Token_Kind_t Token_Kind_t;
    typedef typename TS::Token_t Token_t;

    virtual ~AbsParser() noexcept = default;

    virtual int parse() = 0;
};
} // namespace yuki::pg