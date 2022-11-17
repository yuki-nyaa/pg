#pragma once
#include<yuki/pg/_debug.hpp>
#include<concepts>
#include<type_traits>
#include<utility>
#include<cassert>
#include<yuki/tmp.hpp>
#include<yuki/pg/core.hpp>

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
    constexpr auto& get() & noexcept {
        static_assert(i<sizeof...(ints),"The requested index is greater than the max.");
        return unit_type<i>::member;
    }
    template<size_t i>
    constexpr const auto& get() const& noexcept {
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

struct TToken_Base {};

template<std::unsigned_integral Token_Kind_t,Token_Kind_t kind_p,typename... Ts>
struct TToken: Semantics_Tuple_Impl_<std::make_index_sequence<sizeof...(Ts)>,Ts...>, TToken_Base {static constexpr Token_Kind_t kind_static = kind_p;};

static_assert(std::is_aggregate_v<TToken<unsigned,100,int,double,const char*,const int&>>,"You need C++17 or later to make `yuki::pg::Semantics_Tuple` an aggregate!");


template<typename TS>
struct Any_TToken{
    typedef typename TS::Token_Kind_t Token_Kind_t;

    constexpr Token_Kind_t kind() const noexcept {return kind_;}
    constexpr Location_Range location_range() const noexcept {return location_range_;}
    constexpr bool empty() const noexcept {return static_cast<bool>(p_base);}

    constexpr Any_TToken() noexcept = default;

    template<typename T> requires TS::template is_token_v<T>
    explicit constexpr Any_TToken(T*& p_token_other,const Location_Range location_range_p={}) noexcept : kind_(T::kind_static),location_range_(location_range_p),p_base(p_token_other) {p_token_other=nullptr;}

    template<typename T> requires TS::template is_token_v<T>
    explicit constexpr Any_TToken(T*&& p_token_other,const Location_Range location_range_p={}) noexcept : kind_(T::kind_static),location_range_(location_range_p),p_base(p_token_other) {p_token_other=nullptr;}

    Any_TToken(const Any_TToken&) = delete;
    constexpr Any_TToken(Any_TToken&& other) noexcept : kind_(other.kind_),location_range_(other.location_range_),p_base(other.p_base) {other.p_base=nullptr;}

    Any_TToken& operator=(const Any_TToken&) = delete;
    Any_TToken& operator=(Any_TToken&& other) noexcept {
        if(this!=&other){
            if(p_base){
                YUKI_PG_TARGET_DBGO_ANY_TOKEN_MOVE_ASSIGN("`any_token` fallback destructor called in move assignment with kind {}.\n",kind_);
                alloc.dynamic_destroy_deallocate(kind_,p_base);
            }
            kind_=other.kind_;
            location_range_=other.location_range_;
            p_base=other.p_base;
            other.p_base=nullptr;
        }
        return *this;
    }

    ~Any_TToken() noexcept {
        if(p_base){
            YUKI_PG_TARGET_DBGO_ANY_TOKEN_DTOR("`any_token` fallback destructor called with kind {}.\n",kind_);
            alloc.dynamic_destroy_deallocate(kind_,p_base);
        }
    }

    friend constexpr void swap(Any_TToken& lhs,Any_TToken& rhs) noexcept {
        using std::swap;
        swap(lhs.kind_,rhs.kind_);
        swap(lhs.p_base,rhs.p_base);
        swap(lhs.location_range,rhs.location_range);
    }

    template<typename T> requires TS::template is_token_v<T>
    void static_destroy_deallocate() noexcept {
        assert(kind_==T::kind_static);
        assert(p_base);
        if constexpr(!std::is_trivially_destructible_v<T>)
            static_cast<T*>(p_base)->~T();
        alloc.template deallocate<T>(p_base);
        p_base=nullptr;
    }

    template<typename T> requires TS::template is_token_v<T>
    T& get() & noexcept {
        assert(kind_==T::kind_static);
        assert(p_base);
        return static_cast<T&>(*p_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    const T& get() const& noexcept {
        assert(kind_==T::kind_static);
        assert(p_base);
        return static_cast<const T&>(*p_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    T&& get() && noexcept {
        assert(kind_==T::kind_static);
        assert(p_base);
        return static_cast<T&&>(*p_base);
    }
    template<typename T> requires TS::template is_token_v<T>
    const T&& get() const&& noexcept {
        assert(kind_==T::kind_static);
        assert(p_base);
        return static_cast<const T&&>(*p_base);
    }

  private:
    Token_Kind_t kind_=-1;
    TToken_Base* p_base=nullptr;
    Location_Range location_range_={};
  public:
    static typename TS::Allocator alloc;
}; // struct Any_TToken

template<typename TS>
typename TS::Allocator Any_TToken<TS>::alloc{};
} // namespace yuki::pg
