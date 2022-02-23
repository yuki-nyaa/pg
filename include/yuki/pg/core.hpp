#pragma once
#include<yuki/pg/debug_.hpp>
#include<concepts>
#include<type_traits>
#include<utility>
#include<cassert>
#include<yuki/tmp.hpp>

namespace yuki::pg{
struct Location_Point{
    size_t line=1;
    size_t column=1;
};
struct Location_Range{
    Location_Point start;
    Location_Point end;
};
inline constexpr Location_Range empty_location_range={};
}

namespace yuki::pg{
template<size_t index_p,typename T>
struct Semantics_Tuple_Unit_ {T member;};

template<typename T,typename... Ts>
struct Semantics_Tuple_Impl_;
template<size_t... ints,typename... Ts>
struct Semantics_Tuple_Impl_<std::index_sequence<ints...>,Ts...> : Semantics_Tuple_Unit_<ints,Ts>... {
    static constexpr size_t size = sizeof...(ints);

    template<size_t i>
    using member_type=yuki::type_switch_t<i,Ts...>;
    template<size_t i>
    using unit_type=yuki::type_switch_t<i,Semantics_Tuple_Unit_<ints,Ts>...>;

    template<size_t i>
    constexpr member_type<i>& get() & noexcept {
        static_assert(i<sizeof...(ints),"The requested index is greater than the max.");
        return unit_type<i>::member;
    }
    template<size_t i>
    constexpr const member_type<i>& get() const& noexcept {
        static_assert(i<sizeof...(ints),"The requested index is greater than the max.");
        return unit_type<i>::member;
    }
    template<size_t i>
    constexpr member_type<i>&& get() && noexcept {
        static_assert(i<sizeof...(ints),"The requested index is greater than the max.");
        return static_cast<member_type<i>&&>(unit_type<i>::member);
    }
    template<size_t i>
    constexpr const member_type<i>&& get() const&& noexcept {
        static_assert(i<sizeof...(ints),"The requested index is greater than the max.");
        return static_cast<const member_type<i>&&>(unit_type<i>::member);
    }

    template<typename U>
    constexpr U& get() & noexcept {
        static_assert(yuki::is_any_of_v<U,Ts...>,"The requested type is not among the member types!");
        static_assert(yuki::is_unique_v<U,Ts...>,"The requested type is not unique!");
        return Semantics_Tuple_Unit_<yuki::type_to_num_v<U,Ts...>,U>::member;
    }
    template<typename U>
    constexpr const U& get() const& noexcept {
        static_assert(yuki::is_any_of_v<U,Ts...>,"The requested type is not among the member types!");
        static_assert(yuki::is_unique_v<U,Ts...>,"The requested type is not unique!");
        return Semantics_Tuple_Unit_<yuki::type_to_num_v<U,Ts...>,U>::member;
    }
    template<typename U>
    constexpr U&& get() && noexcept {
        static_assert(yuki::is_any_of_v<U,Ts...>,"The requested type is not among the member types!");
        static_assert(yuki::is_unique_v<U,Ts...>,"The requested type is not unique!");
        return static_cast<U&&>(Semantics_Tuple_Unit_<yuki::type_to_num_v<U,Ts...>,U>::member);
    }
    template<typename U>
    constexpr const U&& get() const&& noexcept {
        static_assert(yuki::is_any_of_v<U,Ts...>,"The requested type is not among the member types!");
        static_assert(yuki::is_unique_v<U,Ts...>,"The requested type is not unique!");
        return static_cast<const U&&>(Semantics_Tuple_Unit_<yuki::type_to_num_v<U,Ts...>,U>::member);
    }
}; // struct Semantics_Tuple_Impl_<std::index_sequence<ints...>,Ts...>

template<>
struct Semantics_Tuple_Impl_<std::index_sequence<>> {
    static constexpr size_t size = 0;
};

template<typename T>
struct Semantics_Tuple_Impl_<std::index_sequence<0>,T> {
    T zeroth;
    static constexpr size_t size = 1;
};

template<typename T0,typename T1>
struct Semantics_Tuple_Impl_<std::index_sequence<0,1>,T0,T1> {
    T0 zeroth;
    T1 first;
    static constexpr size_t size = 2;
};

template<typename T0,typename T1,typename T2>
struct Semantics_Tuple_Impl_<std::index_sequence<0,1,2>,T0,T1,T2> {
    T0 zeroth;
    T1 first;
    T2 second;
    static constexpr size_t size = 3;
};

inline constexpr size_t token_specialization_max_size = 3;

struct Token_Base {};

template<std::unsigned_integral Token_Kind_t,Token_Kind_t kind_p,typename... Ts>
struct Token : Semantics_Tuple_Impl_<std::make_index_sequence<sizeof...(Ts)>,Ts...>, Token_Base {static constexpr Token_Kind_t kind_static = kind_p;};

static_assert(std::is_aggregate_v<Token<unsigned,100,int,double,const char*,const int&>>,"You need C++17 or later to make `yuki::pg::Semantics_Tuple` an aggregate!");

template<typename T>
struct is_token : std::false_type {};
template<std::unsigned_integral Token_Kind_t,Token_Kind_t kind_p,typename... Ts>
struct is_token<Token<Token_Kind_t,kind_p,Ts...>> : std::true_type {};
template<typename T>
inline constexpr bool is_token_v = is_token<T>::value;



template<typename TS,bool = TS::is_simple_token>
struct Any_Token;

template<typename TS>
struct Any_Token<TS,false>{
  public:
    typedef TS Token_Settings;
    typedef typename TS::Token_Kind_t Token_Kind_t;

    constexpr Token_Kind_t kind() const noexcept {return kind_;}

    constexpr Any_Token() noexcept = default;

    template<typename T> requires TS::template is_token_v<T>
    explicit constexpr Any_Token(T*& p_token_other,const Location_Range& location_range_p=empty_location_range) noexcept : kind_(T::kind_static),location_range(location_range_p),p_token_base(p_token_other) {p_token_other=nullptr;}

    template<typename T> requires TS::template is_token_v<T>
    explicit constexpr Any_Token(T*&& p_token_other,const Location_Range& location_range_p=empty_location_range) noexcept : kind_(T::kind_static),location_range(location_range_p),p_token_base(p_token_other) {p_token_other=nullptr;}

    Any_Token(const Any_Token&) = delete;
    constexpr Any_Token(Any_Token&& other) noexcept : kind_(other.kind_),location_range(other.location_range),p_token_base(other.p_token_base) {other.p_token_base=nullptr;}

    Any_Token& operator=(const Any_Token&) = delete;
    Any_Token& operator=(Any_Token&& other) noexcept {
        if(this!=&other){
            if(p_token_base){
                YUKI_PG_TARGET_DBGO_ANY_TOKEN_MOVE_ASSIGN("`any_token` fallback destructor called in move assignment with kind {}.\n",kind_);
                alloc.dynamic_destroy_deallocate(kind_,p_token_base);
            }
            kind_=other.kind_;
            location_range=other.location_range;
            p_token_base=other.p_token_base;
            other.p_token_base=nullptr;
        }
        return *this;
    }
    ~Any_Token() noexcept {
        if(p_token_base){
            YUKI_PG_TARGET_DBGO_ANY_TOKEN_DTOR("`any_token` fallback destructor called with kind {}.\n",kind_);
            alloc.dynamic_destroy_deallocate(kind_,p_token_base);
        }
    }

    template<typename T> requires TS::template is_token_v<T>
    void static_destroy_deallocate() noexcept {
        assert(kind_==T::kind_static);
        if constexpr(!std::is_trivially_destructible_v<T>)
            static_cast<T*>(p_token_base)->~T();
        alloc.template deallocate<T>(p_token_base);
        p_token_base=nullptr;
    }

    template<typename T> requires TS::template is_token_v<T>
    T& get() & noexcept {
        assert(kind_==T::kind_static);
        return static_cast<T&>(*p_token_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    const T& get() const& noexcept {
        assert(kind_==T::kind_static);
        return static_cast<const T&>(*p_token_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    T&& get() && noexcept {
        assert(kind_==T::kind_static);
        return static_cast<T&&>(*p_token_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    const T&& get() const&& noexcept {
        assert(kind_==T::kind_static);
        return static_cast<const T&&>(*p_token_base);
    }

    constexpr bool empty() const noexcept {return p_token_base;}
  private:
    Token_Kind_t kind_=TS::Token_Kind::EOF_;
    Token_Base* p_token_base=nullptr;
  public:
    Location_Range location_range={};

    static typename TS::Allocator alloc;
}; // struct Any_Token

template<typename TS>
typename TS::Allocator Any_Token<TS,false>::alloc{};



template<std::unsigned_integral Token_Kind_t,typename T>
struct Simple_Token_{
    Token_Kind_t kind = -1;
    T value;
    Location_Range location_range={};
};

template<std::unsigned_integral Token_Kind_t>
struct Simple_Token_<Token_Kind_t,void>{
    Token_Kind_t kind = -1;
    Location_Range location_range={};
};

static_assert(std::is_aggregate_v<Simple_Token_<unsigned,Location_Range>>);

template<typename TS>
struct Any_Token<TS,true> : Simple_Token_<typename TS::Token_Kind_t,typename TS::value_type> {};



template<typename TS>
struct AbsParser{
    typedef TS Token_Settings;

    typedef yuki::pg::Any_Token<TS> Any_Token;
    typedef typename TS::Token Token;
    typedef typename TS::Token_Kind Token_Kind;
    typedef typename TS::Token_Kind_t Token_Kind_t;

    virtual ~AbsParser() noexcept = default;

    virtual int parse() = 0;
};
} // namespace yuki::pg