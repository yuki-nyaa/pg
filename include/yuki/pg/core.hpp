#pragma once
#include<cstddef>
#include<cstdint>

namespace yuki::pg{

typedef uint_least32_t location_base_type;

struct Location_Point{
    location_base_type line=0;
    location_base_type column=0;
};

struct Location_Range{
    Location_Point begin={};
    Location_Point end={};
};

enum struct Token_Impl_Type : unsigned char {VARIANT,SIMPLE,TUPLE};

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