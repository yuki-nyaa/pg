// Common definitions for both the lexer and parser, e.g. Token types, the symbol table(s).
#pragma once
#include<type_traits>
#include<cassert>
#include<yuki/tmp.hpp>
#include<yuki/pg/core.hpp>

#include<yuki/allocator.hpp>

#include<list>
namespace xxx{
struct Token_Settings{
    static constexpr bool is_simple_token = true;

    static constexpr size_t token_total=5;
    typedef yuki::uint_auto_t<token_total> Token_Kind_t;
    struct Token_Kind{
        enum enum_t : Token_Kind_t {List,Pair, LB,RB,EOF_}; // This might look sorta ugly. The reason is that I want the enumerators be scoped, while at the same time I don't want to prohibit implicit conversion FROM integers so that in the parsing table definition I can simply write numbers instead of the tedious `Token_Kind::XXX` form. Of course in normal cases magic numbers should be avoided as much as possible for the purpose of readibility, but in the case of a program-generated file I suppose this is not that important.
    };
    const char* token_name[token_total]={"List","Pair","(",")","EOF"}; // Used to print error messages.

    static constexpr Token_Kind_t terminal_first=Token_Kind::LB;
    static constexpr Token_Kind_t terminal_last=Token_Kind::EOF_;
    static constexpr Token_Kind_t eof_=Token_Kind::EOF_;
    static constexpr Token_Kind_t nterminal_first=Token_Kind::List;
    static constexpr Token_Kind_t nterminal_last=Token_Kind::Pair;

    static constexpr size_t terminal_total = terminal_last-terminal_first+1;
    static constexpr size_t nterminal_total = nterminal_last-nterminal_first+1;
    static_assert(terminal_total+nterminal_total==token_total);

    static constexpr bool is_terminal_f(Token_Kind_t kind) {return kind>=terminal_first && kind<=terminal_last;}
    static constexpr bool is_nterminal_f(Token_Kind_t kind) {return kind>=nterminal_first && kind<=nterminal_last;}

    static constexpr size_t terminal_kind_to_index(Token_Kind_t kind){
        assert(is_terminal_f(kind));
        return kind-terminal_first;
    }

    static constexpr size_t nterminal_kind_to_index(Token_Kind_t kind){
        assert(is_nterminal_f(kind));
        return kind-nterminal_first;
    }

    typedef int value_type;
    typedef void Token;
}; // struct Token_Settings
} // namespace xxx
