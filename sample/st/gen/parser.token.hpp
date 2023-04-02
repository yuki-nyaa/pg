#pragma once
#include<list>
#include<cassert>
#include<yuki/tmp.hpp>
#include<yuki/pg/core.hpp>
#include<yuki/pg/TToken.hpp>
#include<yuki/Allocator.hpp>

namespace xxx{
struct Token_Settings{
    static constexpr yuki::pg::Token_Impl_Type token_impl_type = yuki::pg::Token_Impl_Type::TUPLE;

    static constexpr size_t token_total = 5;
    typedef yuki::uint_auto_t<token_total> Token_Kind_t;
    struct Token_Kind{
        enum enum_t : Token_Kind_t {List,Pair,LB,RB,EOF_,};
    };
    static constexpr const char* token_name[token_total] = {"List","Pair","(",")","EOF_",};

    static constexpr Token_Kind_t nterminal_first = Token_Kind::List;
    static constexpr Token_Kind_t nterminal_last = Token_Kind::Pair;
    static constexpr Token_Kind_t terminal_first = Token_Kind::LB;
    static constexpr Token_Kind_t terminal_last = Token_Kind::EOF_;
    static constexpr Token_Kind_t eof_ = Token_Kind::EOF_;

    static constexpr size_t nterminal_total = 2;
    static constexpr size_t terminal_total = 3;

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

    typedef yuki::pg::Any_TToken<Token_Settings> Token_t;

    template<Token_Kind_t kind_p,typename... Ts>
    using TToken_Tp = yuki::pg::TToken<Token_Kind_t,kind_p,Ts...>;

    struct Token{
        using List = TToken_Tp<Token_Kind::List,size_t,std::list<size_t>>;
        using Pair = TToken_Tp<Token_Kind::Pair,size_t,std::list<size_t>>;
        using LB = TToken_Tp<Token_Kind::LB>;
        using RB = TToken_Tp<Token_Kind::RB>;
        using EOF_ = TToken_Tp<Token_Kind::EOF_>;
    }; // struct Token

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
    auto allocate(const size_t n=1) -> typename type_to_alloc_t<T>::pointer {return type_to_alloc_t<T>::allocate(n);}

    template<typename T>
    void deallocate(const yuki::pg::TToken_Base* const p,const size_t n=1) {type_to_alloc_t<T>::deallocate(static_cast<const T*>(p),n);}

    template<typename T>
    void destroy_deallocate(const yuki::pg::TToken_Base* const p){
        if constexpr(!std::is_trivially_destructible_v<T>)
            static_cast<const T*>(p) -> ~T();
        type_to_alloc_t<T>::deallocate(static_cast<const T*>(p));
    }

    void dynamic_destroy_deallocate(const Token_Kind_t k,const yuki::pg::TToken_Base* const p){
        using enum Token_Kind::enum_t;
        switch(k){
            case List : {destroy_deallocate<Token::List>(p);break;}
            case Pair : {destroy_deallocate<Token::Pair>(p);break;}
            case LB : {destroy_deallocate<Token::LB>(p);break;}
            case RB : {destroy_deallocate<Token::RB>(p);break;}
            case EOF_ : {destroy_deallocate<Token::EOF_>(p);break;}
        }
    }
}; // struct Token_Settings::Allocator
} // namespace xxx

