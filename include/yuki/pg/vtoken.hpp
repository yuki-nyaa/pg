#pragma once
#include<concepts>
#include<cassert>
#include<yuki/tmp.hpp>

#include<memory>

#include<yuki/pg/core.hpp>

// Adapted from https://github.com/groundswellaudio/swl-variant

namespace yuki::pg{

namespace vtoken_impl_{
    /// dummy
    struct dm {static constexpr size_t elements = 1;};

    /// union of elements
    template<typename A,typename B>
    union uoe{
        A a;
        B b;

        static constexpr size_t elements = 2 ;


        constexpr uoe() noexcept = default;

        //constexpr uoe(const uoe&) noexcept requires (std::is_trivially_copy_constructible_v<A> && std::is_trivially_copy_constructible_v<B>) = default;
        constexpr uoe(const uoe&) noexcept {}

        //constexpr uoe(uoe&&) noexcept requires (std::is_trivially_move_constructible_v<A> && std::is_trivially_move_constructible_v<B>) = default;
        constexpr uoe(uoe&&) noexcept = default;

        constexpr ~uoe() noexcept requires (std::is_trivially_destructible_v<A> && std::is_trivially_destructible_v<B>) = default;
        constexpr ~uoe() noexcept {}

        //constexpr uoe& operator=(const uoe&) noexcept requires (std::is_trivially_copy_assignable_v<A> && std::is_trivially_copy_assignable_v<B>) = default;
        constexpr uoe& operator=(const uoe&) noexcept = default;

        //constexpr uoe& operator=(uoe&&) noexcept requires (std::is_trivially_move_assignable_v<A> && std::is_trivially_move_assignable_v<B>) = default;
        constexpr uoe& operator=(uoe&&) noexcept = default;

        template<typename... Args>
        explicit constexpr uoe(std::in_place_index_t<0>,Args&&... args) noexcept : a(std::forward<Args>(args)...) {}
        template<typename... Args>
        explicit constexpr uoe(std::in_place_index_t<1>,Args&&... args) noexcept : b(std::forward<Args>(args)...) {}

        template<size_t I>
        constexpr auto& get() & {
            if constexpr(I==0) return a;
            else return b;
        }

        template<size_t I>
        constexpr const auto& get() const& {
            if constexpr(I==0) return a;
            else return b;
        }

        template<size_t I>
        constexpr auto&& get() && {
            if constexpr(I==0) return a;
            else return b;
        }

        template<size_t I>
        constexpr const auto&& get() const&& {
            if constexpr(I==0) return a;
            else return b;
        }
    };

    /// union of unions
    template<typename U0,typename U1>
    union uou{
        U0 u0;
        U1 u1;

        static constexpr size_t elements = U0::elements + U1::elements;
        static constexpr size_t elements0 = elements-1;


        constexpr uou() noexcept = default;

        //constexpr uou(const uou&) noexcept requires (std::is_trivially_copy_constructible_v<U0> && std::is_trivially_copy_constructible_v<U1>) = default;
        constexpr uou(const uou&) noexcept = default;

        //constexpr uou(uou&&) noexcept requires (std::is_trivially_move_constructible_v<U0> && std::is_trivially_move_constructible_v<U1>) = default;
        constexpr uou(uou&&) noexcept = default;

        constexpr ~uou() noexcept requires (std::is_trivially_destructible_v<U0> && std::is_trivially_destructible_v<U1>) = default;
        constexpr ~uou() noexcept {}

        //constexpr uou& operator=(const uou&) noexcept requires (std::is_trivially_copy_assignable_v<U0> && std::is_trivially_copy_assignable_v<U1>) = default;
        constexpr uou& operator=(const uou&) noexcept = default;

        //constexpr uou& operator=(uou&&) noexcept requires (std::is_trivially_move_assignable_v<U0> && std::is_trivially_move_assignable_v<U1>) = default;
        constexpr uou& operator=(uou&&) noexcept = default;

        template<size_t I,typename... Args> requires (I<=elements0/2)
        explicit constexpr uou(std::in_place_index_t<I>,Args&&... args) noexcept : u0(std::in_place_index<I>,std::forward<Args>(args)...) {}
        template<size_t I,typename... Args>
        explicit constexpr uou(std::in_place_index_t<I>,Args&&... args) noexcept : u1(std::in_place_index<I-(elements0/2)-1>,std::forward<Args>(args)...) {}

        template<size_t I>
        constexpr auto& get() & {
            if constexpr(I<=elements0/2) return u0.template get<I>();
            else return u1.template get<I-(elements0/2)-1>();
        }

        template<size_t I>
        constexpr const auto& get() const& {
            if constexpr(I<=elements0/2) return u0.template get<I>();
            else return u1.template get<I-(elements0/2)-1>();
        }

        template<size_t I>
        constexpr auto&& get() && {
            if constexpr(I<=elements0/2) return u0.template get<I>();
            else return u1.template get<I-(elements0/2)-1>();
        }

        template<size_t I>
        constexpr const auto&& get() const&& {
            if constexpr(I<=elements0/2) return u0.template get<I>();
            else return u1.template get<I-(elements0/2)-1>();
        }
    };

    // This should really be an alias template, if alias template can be specialized.
    template<bool is_first_pass,size_t remaining,typename... Ts>
    struct tree_union_helper;

    template<size_t remaining,typename A,typename B,typename... Ts> requires (remaining>=2)
    struct tree_union_helper<true,remaining,A,B,Ts...>{
        using type = typename tree_union_helper<true,remaining-2,Ts...,uoe<A,B>>::type;
    };
    template<typename A,typename... Ts>
    struct tree_union_helper<true,1,A,Ts...>{
        using type = typename tree_union_helper<false,sizeof...(Ts)+1,Ts...,uoe<A,dm>>::type;
    };
    template<typename... Ts>
    struct tree_union_helper<true,0,Ts...>{
        using type = typename tree_union_helper<false,sizeof...(Ts),Ts...>::type;
    };

    template<size_t remaining,typename A,typename B,typename... Ts> requires (remaining>=2)
    struct tree_union_helper<false,remaining,A,B,Ts...>{
        using type = typename tree_union_helper<false,remaining-2,Ts...,uou<A,B>>::type;
    };
    template<typename A,typename... Ts>
    struct tree_union_helper<false,1,A,Ts...>{
        using type = typename tree_union_helper<false,sizeof...(Ts)+1,Ts...,uou<A,dm>>::type;
    };
    template<typename... Ts>
    struct tree_union_helper<false,0,Ts...>{
        using type = typename tree_union_helper<false,sizeof...(Ts),Ts...>::type;
    };

    template<typename T>
    struct tree_union_helper<false,1,T>{
        using type = T;
    };

    template<typename... Ts>
    using tree_union = typename tree_union_helper<true,sizeof...(Ts),Ts...>::type;

    /*
    As an example:
        A B C D E -> C D E <A,B> -> E <A,B> <C,D> -> <A,B> <C,D> <E,dm> ->
        <E,dm> <<A,B>,<C,D>> -> <<A,B>,<C,D>> <<E,dm>,dm> ->
        <<<A,B>,<C,D>>,<<E,dm>,dm>>
    */


    #define YUKI_PG_VTOKEN_4(n,X) X(n) X(n+1) X(n+2) X(n+3)
    #define YUKI_PG_VTOKEN_8(n,X) YUKI_PG_VTOKEN_4(n,X) YUKI_PG_VTOKEN_4(n+4,X)
    #define YUKI_PG_VTOKEN_16(n,X) YUKI_PG_VTOKEN_4(n,X) YUKI_PG_VTOKEN_4(n+4,X) YUKI_PG_VTOKEN_4(n+8,X) YUKI_PG_VTOKEN_4(n+12,X)
    #define YUKI_PG_VTOKEN_32(n,X) YUKI_PG_VTOKEN_8(n,X) YUKI_PG_VTOKEN_8(n+8,X) YUKI_PG_VTOKEN_8(n+16,X) YUKI_PG_VTOKEN_8(n+24,X)
    #define YUKI_PG_VTOKEN_64(n,X) YUKI_PG_VTOKEN_16(n,X) YUKI_PG_VTOKEN_16(n+16,X) YUKI_PG_VTOKEN_16(n+32,X) YUKI_PG_VTOKEN_16(n+48,X)
    #define YUKI_PG_VTOKEN_128(n,X) YUKI_PG_VTOKEN_32(n,X) YUKI_PG_VTOKEN_32(n+32,X) YUKI_PG_VTOKEN_32(n+64,X) YUKI_PG_VTOKEN_32(n+96,X)
    #define YUKI_PG_VTOKEN_256(n,X) YUKI_PG_VTOKEN_64(n,X) YUKI_PG_VTOKEN_64(n+64,X) YUKI_PG_VTOKEN_64(n+128,X) YUKI_PG_VTOKEN_64(n+192,X)

    #define YUKI_PG_VTOKEN_SINGLE_VISIT_CASE(n) \
        case (n) : \
    		if constexpr(n < V_nocvref::size) \
			    return std::forward<F>(f)(std::forward<V>(v).template get<n>(),std::forward<Vs>(vs).template get<n>()...); \

    #define YUKI_PG_VTOKEN_SINGLE_VISIT_N(n) \
        template<typename R,typename F,typename V,typename... Vs> \
        constexpr R single_visit_##n(F&& f,V&& v,Vs&&... vs){ \
            typedef std::remove_cvref_t<V> V_nocvref; \
            switch(v.type_index()){ \
                YUKI_PG_VTOKEN_##n(0,YUKI_PG_VTOKEN_SINGLE_VISIT_CASE) \
            } \
            __builtin_unreachable();\
        }

    YUKI_PG_VTOKEN_SINGLE_VISIT_N(4)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(8)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(16)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(32)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(64)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(128)
    YUKI_PG_VTOKEN_SINGLE_VISIT_N(256)

    #undef YUKI_PG_VTOKEN_SINGLE_VISIT_N
    #undef YUKI_PG_VTOKEN_SINGLE_VISIT_CASE
    #undef YUKI_PG_VTOKEN_256
    #undef YUKI_PG_VTOKEN_128
    #undef YUKI_PG_VTOKEN_64
    #undef YUKI_PG_VTOKEN_32
    #undef YUKI_PG_VTOKEN_16
    #undef YUKI_PG_VTOKEN_8
    #undef YUKI_PG_VTOKEN_4

    template<size_t i,typename R,typename F,typename V,typename... Vs>
    constexpr R visit_at(F&& f,V&& v,Vs&&... vs) {return std::forward<F>(f)(std::forward<V>(v).template get<i>(),std::forward<Vs>(vs).template get<i>()...);}

    template<typename Seq,typename R,typename F,typename V,typename... Vs>
    struct single_visit_by_table;

    template<typename R,typename F,typename V,typename... Vs,size_t... is>
    struct single_visit_by_table<std::index_sequence<is...>,R,F,V,Vs...>{
        static constexpr R (*table[sizeof...(is)])(F&&,V&&,Vs&&...) = {&visit_at<is,R,F,V,Vs...>...};
        static constexpr R visit(F&& f,V&& v,Vs&&... vs) {return table[v.type_index()](std::forward<F>(f),std::forward<V>(v),std::forward<Vs>(vs)...);}
    };

    template<typename R,typename F,typename V,typename... Vs>
    constexpr R visit_homogenous(F&& f,V&& v,Vs&&... vs){
        {
        const auto i = v.type_index();
        assert((... && (vs.type_index()==i)));
        }

        typedef std::remove_cvref_t<V> V_nocvref;

        #define YUKI_PG_VTOKEN_VISITH_BRANCH(n) \
            else if constexpr(V_nocvref::size<=n) \
                return vtoken_impl_::single_visit_##n<R>(std::forward<F>(f),std::forward<V>(v),std::forward<Vs>(vs)...);

        if constexpr(V_nocvref::size==1)
            return std::forward<F>(f)(std::forward<V>(v).template get<0>(),std::forward<Vs>(vs).template get<0>()...);
        YUKI_PG_VTOKEN_VISITH_BRANCH(4)
        YUKI_PG_VTOKEN_VISITH_BRANCH(8)
        YUKI_PG_VTOKEN_VISITH_BRANCH(16)
        YUKI_PG_VTOKEN_VISITH_BRANCH(32)
        YUKI_PG_VTOKEN_VISITH_BRANCH(64)
        YUKI_PG_VTOKEN_VISITH_BRANCH(128)
        YUKI_PG_VTOKEN_VISITH_BRANCH(256)
        else
            return vtoken_impl_::single_visit_by_table<std::make_index_sequence<V_nocvref::size>,R,F,V,Vs...>::visit(std::forward<F>(f),std::forward<V>(v),std::forward<Vs>(vs)...);

        #undef YUKI_PG_VTOKEN_VISITH_BRANCH
    }


    inline constexpr struct destroy_visitor_t{
        template<typename T>
        constexpr void operator()(T&& t) const {
            typedef std::remove_cvref_t<T> T_nocvref;
            t.~T_nocvref();
        }
    } destroy_visitor;

    inline constexpr struct copy_visitor_t{
        template<typename Vt,typename Vs>
        constexpr void operator()(Vt& vt,Vs&& vs) const {
            if constexpr(std::is_constructible_v<Vt,Vs&&>)
                std::construct_at(std::addressof(vt),std::forward<Vs>(vs));
            else
                assert(false); // Attempt to copy/move a non-copy(move)able alternative.
        }
    } copy_visitor;

    inline constexpr struct assign_visitor_t{
        template<typename Vt,typename Vs>
        constexpr void operator()(Vt&& vt,Vs&& vs) const {
            if constexpr(std::is_assignable_v<Vt&&,Vs&&>)
                std::forward<Vt>(vt)=std::forward<Vs>(vs);
            else
                assert(false); // Attempt to copy/move assign a non-copy(move)-assignable alternative.
        }
    } assign_visitor;

    inline constexpr struct swap_visitor_t{
        template<typename Vt,typename Vs>
        constexpr void operator()(Vt&& vt,Vs&& vs) const {
            if constexpr(std::is_swappable_with_v<Vt&&,Vs&&>){
                using std::swap;
                swap(std::forward<Vt>(vt),std::forward<Vs>(vs));
            }else
                assert(false); // Attempt to swap a non-swappable alternative.
        }
    } swap_visitor;
} // namespace vtoken_impl_


template<auto K>
struct in_place_kind_t {};
template<auto K>
inline constexpr in_place_kind_t<K> in_place_kind;


template<std::unsigned_integral Token_Kind_t,typename TIT,typename... Ts>
struct VToken : private TIT {
  public:
    using TIT::token_index_table;
    static constexpr size_t size = sizeof...(Ts) + 1;
  private:
    vtoken_impl_::tree_union<vtoken_impl_::dm,Ts...> tu_;
    Token_Kind_t kind_;
    Location_Range location_range_;
  public:
    constexpr Token_Kind_t kind() const {return kind_;}
    constexpr Location_Range location_range() const {return location_range_;}
    constexpr Token_Kind_t type_index() const {return token_index_table[kind_];}

    constexpr VToken() noexcept requires (... && std::is_trivially_default_constructible_v<Ts>) = default;
    constexpr VToken() noexcept :
        tu_(std::in_place_index<0>),
        kind_(-1),
        location_range_{}
    {}

    constexpr VToken(const VToken&) noexcept requires (... && std::is_trivially_copy_constructible_v<Ts>) = default;
    constexpr VToken(const VToken& other) noexcept :
        tu_(std::in_place_index<0>),
        kind_(other.kind_),
        location_range_(other.location_range_)
    {vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,*this,other);}

    constexpr VToken(VToken&&) noexcept requires (... && std::is_trivially_move_constructible_v<Ts>) = default;
    constexpr VToken(VToken&& other) noexcept :
        tu_(std::in_place_index<0>),
        kind_(other.kind_),
        location_range_(other.location_range_)
    {vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,*this,std::move(other));}

    constexpr ~VToken() noexcept requires (... && std::is_trivially_destructible_v<Ts>) = default;
    constexpr ~VToken() noexcept {vtoken_impl_::visit_homogenous<void>(vtoken_impl_::destroy_visitor,*this);}

    constexpr VToken& operator=(const VToken&) noexcept requires (... && std::is_trivially_copy_assignable_v<Ts>) = default;
    constexpr VToken& operator=(const VToken& other) noexcept {
        if(this!=&other){
            if(kind_==other.kind_){
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::assign_visitor,*this,other);
                location_range_ = other.location_range_;
            }else{
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::destroy_visitor,*this);
                kind_=other.kind_;
                location_range_ = other.location_range_;
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,*this,other);
            }
        }
        return *this;
    }

    constexpr VToken& operator=(VToken&&) noexcept requires (... && std::is_trivially_move_assignable_v<Ts>) = default;
    constexpr VToken& operator=(VToken&& other) noexcept {
        if(this!=&other){
            if(kind_==other.kind_){
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::assign_visitor,*this,std::move(other));
                location_range_ = other.location_range_;
            }else{
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::destroy_visitor,*this);
                kind_=other.kind_;
                location_range_ = other.location_range_;
                vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,*this,std::move(other));
            }
        }
        return *this;
    }

    friend constexpr void swap(VToken& lhs,VToken& rhs) noexcept {
        using std::swap;
        if(lhs.active_==rhs.active_){
            vtoken_impl_::visit_homogenous<void>(vtoken_impl_::swap_visitor,lhs,rhs);
            swap(lhs.location_range_,rhs.location_range_);
        }else{
            swap(lhs.location_range_,rhs.location_range_);
            VToken temp = std::move(lhs);
            vtoken_impl_::visit_homogenous<void>(vtoken_impl_::destroy_visitor,lhs);
            lhs.kind_=rhs.kind_;
            vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,lhs,std::move(rhs));
            vtoken_impl_::visit_homogenous<void>(vtoken_impl_::destroy_visitor,rhs);
            rhs.kind_=temp.kind_;
            vtoken_impl_::visit_homogenous<void>(vtoken_impl_::copy_visitor,rhs,std::move(temp));
        }
    }

    template<auto K,typename... Args>
    explicit constexpr VToken(yuki::pg::in_place_kind_t<K>,Args&&... args) noexcept :
        tu_(std::in_place_index<token_index_table[K]>,std::forward<Args>(args)...),
        kind_(K)
    {}

    /// @tparam I Type index, not kind!!
    template<Token_Kind_t I>
    constexpr decltype(auto) get() & {assert(I==token_index_table[kind_]);return tu_.template get<I>();}
    template<Token_Kind_t I>
    constexpr decltype(auto) get() const& {assert(I==token_index_table[kind_]);return tu_.template get<I>();}
    template<Token_Kind_t I>
    constexpr decltype(auto) get() && {assert(I==token_index_table[kind_]);return tu_.template get<I>();}
    template<Token_Kind_t I>
    constexpr decltype(auto) get() const&& {assert(I==token_index_table[kind_]);return tu_.template get<I>();}
};

} // namespace yuki::pg


/*
Implementation notes:

Performing a construction-visit after a destruction-visit actually is a UB, since the member function `.get<J>` is called on a union whose lifetime has already been ended. However, it looks like a non-UB approach would involve either a `reinterpret_cast` or a `new` expression, any of which is explicitly forbidden to appear in a `constexpr` function. So in short, either "UB and constexpr" or "no UB but no constexpr".
(This is not entirely correct, though, since a UB also cannot be `constexpr`. But since GCC 12.1 does not complain and produces desired results, I've decided to choose the UB approach.)
(Clang 14.0 seems to be unable to perform the correct overload resolution among constrained functions so I am unable to test the code with it.)
*/