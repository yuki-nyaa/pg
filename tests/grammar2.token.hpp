#pragma once

#include<cassert>
#include<yuki/tmp.hpp>
#include<yuki/pg/core.hpp>
#include<yuki/pg/VToken.hpp>

struct Token_Settings{
    static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::VARIANT;

    static constexpr size_t token_total = 14;
    typedef yuki::uint_auto_t<token_total> Token_Kind_t;
    struct Token_Kind{
        enum enum_t : Token_Kind_t {X,Y,Z,T,W,V,a,b,c,d,e,t,u,EOF_,};
    };
    static constexpr const char* token_name[token_total] = {"X","Y","Z","T","W","V","a","b","c","d","e","t","u","EOF_",};

    static constexpr Token_Kind_t nterminal_first = Token_Kind::X;
    static constexpr Token_Kind_t nterminal_last = Token_Kind::V;
    static constexpr Token_Kind_t terminal_first = Token_Kind::a;
    static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;
    static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;

    static constexpr size_t nterminal_total = 6;
    static constexpr size_t terminal_total = 8;

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

    static constexpr Token_Kind_t token_index_table[14] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    }; // token_index_table

    typedef yuki::pg::VToken<Token_Kind_t,token_index_table
    > Token_t;
    typedef void Token;
}; // struct Token_Settings

