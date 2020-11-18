// Facilities for all kinds of parsers.
// TODO noexcept fine tunning
// TODO debug output
#pragma once
#include<utility>
#include<type_traits>
#include<concepts>
#include<cassert>
#include<yuki/utils.hpp>
namespace yuki::pg{
    // Token types.
    template<std::unsigned_integral Token_Kind_t>
    struct Token_Base_General{
        public:
            const Token_Kind_t kind;
            virtual constexpr ~Token_Base_General() noexcept = 0;
            constexpr void static_destruct() noexcept {}
        protected:
            constexpr Token_Base_General(Token_Kind_t kind_con) noexcept : kind(kind_con) {}
    };
    template<std::unsigned_integral Token_Kind_t>
    constexpr Token_Base_General<Token_Kind_t>::~Token_Base_General<Token_Kind_t>() noexcept = default;

    template<typename Token_Kind_t,Token_Kind_t kind,typename... Ss>
    struct Token_Concrete_General;

    // No semantic values.
    template<typename Token_Kind_t,Token_Kind_t k>
    struct Token_Concrete_General<Token_Kind_t,k> final : Token_Base_General<Token_Kind_t>{
        public:
            typedef Token_Base_General<Token_Kind_t> Base_;

            constexpr Token_Concrete_General() noexcept : Base_(k) {}
            constexpr Token_Concrete_General(const Token_Concrete_General&) noexcept : Base_(k) {}
            constexpr Token_Concrete_General& operator=(const Token_Concrete_General&) noexcept {}
            constexpr Token_Concrete_General(Token_Concrete_General&&) noexcept : Base_(k) {}
            constexpr Token_Concrete_General& operator=(Token_Concrete_General&&) noexcept {}
            virtual constexpr ~Token_Concrete_General() noexcept = default;

            constexpr void static_destruct() noexcept {}
    };

    // 2 semantic values.
    template<typename Token_Kind_t, Token_Kind_t k, typename T0, typename T1>
    struct Token_Concrete_General<Token_Kind_t,k,T0,T1> final : Token_Base_General<Token_Kind_t>{
        public:
            typedef Token_Base_General<Token_Kind_t> Base_;

            T0 s0;
            T1 s1;

            template<size_t index>
            using index_to_type = yuki::type_switch_t<index,T0,T1>;

            constexpr Token_Concrete_General() noexcept : Base_(k), s0(), s1() {}
            constexpr Token_Concrete_General(const Token_Concrete_General& rhs) noexcept(noexcept(T0(rhs.s0)) && noexcept(T1(rhs.s1))) : Base_(k), s0(rhs.s0), s1(rhs.s1) {}
            constexpr Token_Concrete_General& operator=(const Token_Concrete_General& rhs)
                noexcept(noexcept(Token_Concrete_General(rhs)) && noexcept(swap(*this, rhs))) {
                using std::swap;
                Token_Concrete_General tmp(rhs);
                swap(*this, rhs);
                return *this;
            }
            constexpr Token_Concrete_General(Token_Concrete_General&& rhs) noexcept(noexcept(T0(rhs.s0)) && noexcept(T1(rhs.s1))) : Base_(k), s0(rhs.s0), s1(rhs.s1) {}
            constexpr Token_Concrete_General& operator=(Token_Concrete_General&& rhs){
                s0=rhs.s0;
                s1=rhs.s1;
            }
            virtual constexpr ~Token_Concrete_General() noexcept(std::is_nothrow_destructible_v<T0> && std::is_nothrow_destructible_v<T1>) = default;

            constexpr void static_destruct() noexcept(std::is_nothrow_destructible_v<Token_Concrete_General>) {
                s0.~T0();
                s1.~T1();
            }

            template<typename T_con, typename U_con>
            constexpr Token_Concrete_General(T_con&& s0_rhs, U_con&& s1_rhs)
            noexcept(noexcept(T0(std::forward<T_con>(s0_rhs))) && noexcept(T1(std::forward<U_con>(s1_rhs))))
            : Base_(k), s0(std::forward<T_con>(s0_rhs)), s1(std::forward<U_con>(s1_rhs)) {}
        protected:
            template<typename T_get>
            constexpr T_get& get_helper() const noexcept { // Returns non-const reference anyway. To be put back later.
                if constexpr(std::is_same_v<T_get,T0>){
                    static_assert(yuki::is_unique_v<T0,T1>);
                    return const_cast<T_get&>(s0);
                }else if constexpr(std::is_same_v<T_get,T1>){
                    static_assert(yuki::is_unique_v<T1,T0>);
                    return const_cast<T_get&>(s1);
                }else{
                    static_assert(yuki::static_assert_dummy_v<T_get>,"The requested type does not exist!");
                }
            }
            template<size_t index>
            constexpr index_to_type<index>& get_helper() const noexcept { // Returns non-const reference anyway. To be put back later.
                static_assert(index>=0 && index<=1,"The requested index does not exist!");
                if constexpr(index==0)
                    return const_cast<index_to_type<index>&>(s0);
                else if constexpr(index==1)
                    return const_cast<index_to_type<index>&>(s1);
            }
        public:
            // Compile-time `get` by type.
            template<typename T_get>
            constexpr T_get& get() & noexcept {return get_helper<T_get>();}
            template<typename T_get>
            constexpr const T_get& get() const& noexcept {return const_cast<const T_get&>(get_helper<T_get>());}
            template<typename T_get>
            constexpr T_get&& get() && noexcept {return static_cast<T_get&&>(get_helper<T_get>());}
            template<typename T_get>
            constexpr const T_get&& get() const&& noexcept {return const_cast<const T_get&&>(static_cast<T_get&&>(get_helper<T_get>()));}

            // Compile-time `get` by index.
            template<size_t index>
            constexpr index_to_type<index>& get() & noexcept {return get_helper<index>();}
            template<size_t index>
            constexpr const index_to_type<index>& get() const& noexcept {return const_cast<const index_to_type<index>&>(get_helper<index>());}
            template<size_t index>
            constexpr index_to_type<index>&& get() && noexcept {return static_cast<index_to_type<index>&&>(get_helper<index>());}
            template<size_t index>
            constexpr const index_to_type<index>&& get() const&& noexcept {return const_cast<const index_to_type<index>&&>(static_cast<index_to_type<index>&&>(get_helper<index>()));}
    }; // struct Token_Concrete_General<k,T0,T1>

    template<typename T>
    struct is_token_general : std::false_type {};
    template<typename Token_Kind_t, Token_Kind_t k,typename... Ss>
    struct is_token_general<Token_Concrete_General<Token_Kind_t,k,Ss...>> : std::true_type {};
    template<typename T>
    inline constexpr bool is_token_general_v = is_token_general<T>::value;
    template<typename T>
    concept Token_General_Concept = is_token_general_v<T>;

    template<typename TS,typename T>
    struct is_token : TS::template is_token<T> {};
    template<typename TS,typename T>
    inline constexpr bool is_token_v = is_token<TS,T>::value;
    template<typename TS,typename T>
    concept Token_Concept = is_token_v<TS,T>;

    template<typename TS,typename T>
    struct is_terminal : TS::template is_terminal<T> {};
    template<typename TS,typename T>
    inline constexpr bool is_terminal_v = is_terminal<TS,T>::value;
    template<typename TS,typename T>
    concept Terminal_Concept = is_terminal_v<TS,T>;

    template<typename TS,typename T>
    struct is_nterminal : TS::template is_nterminal<T> {};
    template<typename TS,typename T>
    inline constexpr bool is_nterminal_v = is_nterminal<TS,T>::value;
    template<typename TS,typename T>
    concept NTerminal_Concept = is_nterminal_v<TS,T>;

    template<typename TS>
    struct any_token{
        private:
            Token_Base_General<typename TS::Token_Kind_t>* base_ptr = nullptr;

            constexpr any_token(Token_Base_General<typename TS::Token_Kind_t>* p) noexcept : base_ptr(p) {};
        public:
            template<typename T,typename... Args>
            friend typename TS::any_token TS::make_any_token(Args&&... args);

            constexpr any_token() noexcept : base_ptr(nullptr) {}

            constexpr bool empty() const noexcept {return base_ptr;}

            ~any_token() noexcept(noexcept(delete base_ptr)) {
                if(base_ptr){
                    std::cerr<<"Fallback destructor called with kind "<<base_ptr->kind<<std::endl;
                    delete base_ptr;
                }
            }

            any_token(const any_token& rhs) = delete;
            any_token& operator=(const any_token& rhs) = delete;

            constexpr any_token(any_token&& rhs) noexcept : base_ptr(rhs.base_ptr) { rhs.base_ptr = nullptr; }
            any_token& operator=(any_token&& rhs) noexcept{
                if(std::addressof(rhs)!=this)
                    base_ptr = rhs.base_ptr;
                    rhs.base_ptr = nullptr;
                    return *this;
            }

            template<typename T> requires is_token_v<TS,T>
            void static_free() noexcept(noexcept(std::declval<T>().static_destruct())) {
                assert(TS::template type_to_kind<T> == base_ptr->kind);
                static_cast<T*>(base_ptr)->static_destruct();
                base_ptr=nullptr;
            }

            template<typename T> requires is_token_v<TS,T>
            void free(){
                delete base_ptr;
                base_ptr=nullptr;
            }

            typename TS::Token_Kind_t get_kind() noexcept {return base_ptr->kind;}

            template<typename T> requires is_token_v<TS,T>
            T& static_get() noexcept {
                assert(TS::template type_to_kind<T> == base_ptr->kind);
                return *(static_cast<T*>(base_ptr));
            }

            template<typename T> requires is_token_v<TS,T>
            T& get() {
                return *(dynamic_cast<T*>(base_ptr));
            }
    };

    // Enumeration of parsing algorithms.
    struct Parsing_Algorithm{
        enum Enum : unsigned char {LR1,LALR1,GLR,LL1};
    };
} // namespace yuki::pg
