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
    static constexpr bool is_simple_token = false;

    static constexpr size_t token_total=5;
    typedef yuki::uint_auto_t<token_total> Token_Kind_t;
    struct Token_Kind{
        enum Enum : Token_Kind_t {List,Pair, LB,RB,EOF_}; // This might look sorta ugly. The reason is that I want the enumerators be scoped, while at the same time I don't want to prohibit implicit conversion FROM integers so that in the parsing table definition I can simply write numbers instead of the tedious `Token_Kind::XXX` form. Of course in normal cases magic numbers should be avoided as much as possible for the purpose of readibility, but in the case of a program-generated file I suppose this is not that important.
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

    template<Token_Kind_t kind_p,typename... Ts>
    using Token_Generic = yuki::pg::Token<Token_Kind_t,kind_p,Ts...>;

    struct Token{
        using List = Token_Generic<Token_Kind::List,size_t,std::list<size_t>>;
        using Pair = Token_Generic<Token_Kind::Pair,size_t,std::list<size_t>>;
        using LB = Token_Generic<Token_Kind::LB>;
        using RB = Token_Generic<Token_Kind::RB>;
        using EOF_ = Token_Generic<Token_Kind::EOF_>;
    };

    template<typename T> struct is_nterminal : std::false_type {};
    template<typename T> static constexpr bool is_nterminal_v = is_nterminal<T>::value;
    template<typename T> struct is_terminal : std::false_type {};
    template<typename T> static constexpr bool is_terminal_v = is_terminal<T>::value;
    template<typename T>
    struct is_token : std::conditional_t<is_nterminal_v<T> || is_terminal_v<T>,std::true_type,std::false_type> {};
    template<typename T>
    static constexpr bool is_token_v = is_nterminal_v<T> || is_terminal_v<T>;

    template<Token_Kind_t kind_p> struct kind_to_type {};
    template<Token_Kind_t kind_p> using kind_to_type_t = typename kind_to_type<kind_p>::type;

    template<typename T> struct type_to_alloc {typedef yuki::Array_Allocator<T> type;};
    template<typename T> using type_to_alloc_t = typename type_to_alloc<T>::type;
    struct Allocator;
}; // struct Token_Settings

template<> struct Token_Settings::is_nterminal<Token_Settings::Token::List> : std::true_type {};
template<> struct Token_Settings::is_nterminal<Token_Settings::Token::Pair> : std::true_type {};

template<> struct Token_Settings::is_terminal<Token_Settings::Token::LB> : std::true_type {};
template<> struct Token_Settings::is_terminal<Token_Settings::Token::RB> : std::true_type {};
template<> struct Token_Settings::is_terminal<Token_Settings::Token::EOF_> : std::true_type {};

template<> struct Token_Settings::kind_to_type<Token_Settings::Token_Kind::List> {typedef Token_Settings::Token::List type;};
template<> struct Token_Settings::kind_to_type<Token_Settings::Token_Kind::Pair> {typedef Token_Settings::Token::Pair type;};
template<> struct Token_Settings::kind_to_type<Token_Settings::Token_Kind::LB> {typedef Token_Settings::Token::LB type;};
template<> struct Token_Settings::kind_to_type<Token_Settings::Token_Kind::RB> {typedef Token_Settings::Token::RB type;};
template<> struct Token_Settings::kind_to_type<Token_Settings::Token_Kind::EOF_> {typedef Token_Settings::Token::EOF_ type;};

struct Token_Settings::Allocator :
    private type_to_alloc_t<Token::List>,
    private type_to_alloc_t<Token::Pair>,
    private type_to_alloc_t<Token::LB>,
    private type_to_alloc_t<Token::RB>,
    private type_to_alloc_t<Token::EOF_>
{
    template<typename T>
    auto allocate(size_t n=1) -> typename type_to_alloc_t<T>::pointer {return type_to_alloc_t<T>::allocate(n);}

    template<typename T>
    void deallocate(const yuki::pg::Token_Base* p,size_t n=1) {type_to_alloc_t<T>::deallocate(static_cast<const T*>(p),n);}

    template<typename T>
    void destroy_deallocate(const yuki::pg::Token_Base* p) {
        if constexpr(!std::is_trivially_destructible_v<T>)
            static_cast<const T*>(p) -> ~T();
        type_to_alloc_t<T>::deallocate(static_cast<const T*>(p));
    }

    void dynamic_destroy_deallocate(Token_Kind_t k,const yuki::pg::Token_Base* p){
        switch(k){
            case Token_Kind::List : {destroy_deallocate<Token::List>(p);break;}
            case Token_Kind::Pair : {destroy_deallocate<Token::Pair>(p);break;}
            case Token_Kind::LB : {destroy_deallocate<Token::LB>(p);break;}
            case Token_Kind::RB : {destroy_deallocate<Token::RB>(p);break;}
            case Token_Kind::EOF_ : {destroy_deallocate<Token::EOF_>(p);break;}
        }
    }
};

} // namespace xxx
