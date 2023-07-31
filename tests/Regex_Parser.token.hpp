#pragma once
#include"FSM.hpp"
#include<cassert>
#include<yuki/tmp.hpp>
#include<yuki/pg/core.hpp>
#include<yuki/pg/VToken.hpp>

namespace yuki::lex{
struct Regex_Parser_TS{
    static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::VARIANT;

    static constexpr size_t token_total = 23;
    typedef yuki::uint_auto_t<token_total> Token_Kind_t;
    struct Token_Kind{
        enum enum_t : Token_Kind_t {Regex,Char_Class,Char_Interval,Char_Intervals,amount,character,meta_char,basic_char_class,paren_l,paren_r,sqparen_l,sqparen_r,ast,plus,qmark,qe,lazy,dlazy,vbar,hat,to,esc_N,EOF_,};
    };
    static constexpr const char* token_name[token_total] = {"Regex","Char_Class","Char_Interval","Char_Intervals","amount","character","meta_char","basic_char_class","(",")","[","]","*","+","?","?=","?l:","?d:","|","^","-","esc_N","EOF_",};

    static constexpr Token_Kind_t nterminal_first = Token_Kind::Regex;
    static constexpr Token_Kind_t nterminal_last = Token_Kind::Char_Intervals;
    static constexpr Token_Kind_t terminal_first = Token_Kind::amount;
    static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;
    static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;

    static constexpr size_t nterminal_total = 4;
    static constexpr size_t terminal_total = 19;

    static constexpr bool is_nterminal_f(const Token_Kind_t kind) {return kind>=nterminal_first && kind<=nterminal_last;}
    static constexpr bool is_terminal_f(const Token_Kind_t kind) {return kind>=terminal_first && kind<=terminal_last;}

    static constexpr size_t nterminal_kind_to_index(const Token_Kind_t kind){
        assert(is_nterminal_f(kind));
        return kind-nterminal_first;
    }

    static constexpr size_t terminal_kind_to_index(const Token_Kind_t kind){
        assert(is_terminal_f(kind));
        return kind-terminal_first;
    }

    static constexpr Token_Kind_t token_index_table[23] = {
        1, 2, 3, 2, 4, 5, 6, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    }; // token_index_table

    typedef yuki::pg::VToken<Token_Kind_t,token_index_table
        , FSM, yuki::IntegralCIs_OV<char32_t>, yuki::CInterval<char32_t>, Amount, char32_t, int
    > Token_t;
    typedef void Token;
}; // struct Regex_Parser_TS
} // namespace yuki::lex

