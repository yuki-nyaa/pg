// Common definitions for both the lexer and parser, e.g. Token types, the symbol table(s).
#pragma once
#include<type_traits>
#include<yuki/utils.hpp>
#include<yuki/ParserGen/core.hpp>

#include<list>
namespace xxx{
    struct Token_Settings{
        typedef unsigned int Token_Kind_t;
        static_assert(std::is_unsigned_v<Token_Kind_t> && std::is_integral_v<Token_Kind_t>);

        struct Token_Kind{
            enum Enum : Token_Kind_t {LB,RB,EoF,   List,Pair};
            static constexpr Token_Kind_t Terminal_LAST_ = EoF;
            static constexpr Token_Kind_t NTerminal_LAST_ = Pair;
        };

        static constexpr auto terminal_size = Token_Kind::Terminal_LAST_+1;
        static constexpr auto nterminal_size = Token_Kind::NTerminal_LAST_-Token_Kind::Terminal_LAST_;

        static constexpr bool is_terminal_func(Token_Kind_t kind){ return kind<=Token_Kind::Terminal_LAST_; }
        static constexpr bool is_nterminal_func(Token_Kind_t kind){ return kind>Token_Kind::Terminal_LAST_ && kind<=Token_Kind::NTerminal_LAST_; }

        static constexpr Token_Kind_t terminal_kind_normalize(Token_Kind_t kind){
            assert(kind<=Token_Kind::Terminal_LAST_);
            return kind;
        }

        static constexpr Token_Kind_t nterminal_kind_normalize(Token_Kind_t kind){
            assert(kind>Token_Kind::Terminal_LAST_ && kind<=Token_Kind::NTerminal_LAST_);
            return kind-Token_Kind::Terminal_LAST_-1;
        }

        using Token_Base = yuki::pg::Token_Base_General<Token_Kind_t>;

        template<Token_Kind_t kind,typename... Ss>
        using Token_Concrete = yuki::pg::Token_Concrete_General<Token_Kind_t,kind,Ss...>;

        struct Terminal{
            using LB = Token_Concrete<Token_Kind::LB>;
            using RB = Token_Concrete<Token_Kind::RB>;
            using EoF = Token_Concrete<Token_Kind::EoF>;
        };
        struct NTerminal{
            using List = Token_Concrete<Token_Kind::List,size_t,std::list<size_t>>;
            using Pair = Token_Concrete<Token_Kind::Pair,size_t,std::list<size_t>>;
        };
        template<Token_Kind_t kind>
        using kind_to_type = yuki::type_switch_t<kind,Terminal::LB,Terminal::RB,Terminal::EoF,NTerminal::List,NTerminal::Pair>;

        template<typename T>
        static constexpr size_t type_to_kind_helper_ = yuki::type_to_num_v<T,Terminal::LB,Terminal::RB,Terminal::EoF,NTerminal::List,NTerminal::Pair>;
        template<yuki::pg::Token_General_Concept T>
        static constexpr size_t type_to_kind = type_to_kind_helper_<T>;

        template<typename T>
        struct is_token : std::conditional_t<(type_to_kind_helper_<T> <= Token_Kind::NTerminal_LAST_),std::true_type,std::false_type> {};
        template<typename T>
        static constexpr bool is_token_v = is_token<T>::value;

        template<typename T>
        struct is_terminal : std::conditional_t<(type_to_kind_helper_<T> <= Token_Kind::Terminal_LAST_),std::true_type,std::false_type> {};
        template<typename T>
        static constexpr bool is_terminal_v = is_terminal<T>::value;

        template<typename T>
        struct is_nterminal : std::conditional_t<((type_to_kind_helper_<T> > Token_Kind::Terminal_LAST_) && (type_to_kind_helper_<T> <= Token_Kind::NTerminal_LAST_)),std::true_type,std::false_type> {};
        template<typename T>
        static constexpr bool is_nterminal_v = is_nterminal<T>::value;

        typedef yuki::pg::any_token<Token_Settings> any_token;

        template<typename T,typename... Args>
        [[nodiscard("Discarding this results in memory leak!")]]
        static any_token make_any_token(Args&&... args){
            return any_token(yuki::statik_kast<Token_Base*>(new T(std::forward<Args>(args)...)));
        }
    }; // struct Token_Settings
} // namespace xxx