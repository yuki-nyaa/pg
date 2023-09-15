#pragma once
#include<yuki/Variant.hpp>

namespace yuki::pg{

template<auto K>
struct in_place_kind_t {};
template<auto K>
inline constexpr in_place_kind_t<K> in_place_kind;


/// @tparam token_index_table     Maps "token kind" to "type index".
template<std::unsigned_integral Token_Kind_t,const Token_Kind_t* token_index_table,typename Loc,typename... Ts>
struct VToken : private Loc{
  public:
    static constexpr size_t size = sizeof...(Ts) + 1;
  private:
    yuki::variant_impl_::tree_union<yuki::variant_impl_::dm,Ts...> tu_;
    Token_Kind_t kind_;

    template<typename R,typename F,typename V,typename... Vs>
    friend R yuki::visit_homogenous(F&&,V&&,Vs&&...);

    constexpr Token_Kind_t index() const {return token_index_table[kind_];}
  public:
    constexpr Token_Kind_t kind() const {return kind_;}
    constexpr Token_Kind_t type_index() const {return token_index_table[kind_];}
    constexpr Loc& location() {return *this;}
    constexpr const Loc& location() const {return *this;}

    constexpr VToken(const VToken&) noexcept requires (... && std::is_trivially_copy_constructible_v<Ts>) = default;
    constexpr VToken(const VToken& other) noexcept :
        Loc(static_cast<const Loc&>(other)),
        tu_(std::in_place_index<0>),
        kind_(other.kind_)
    {yuki::visit_homogenous<void>(yuki::copy_visitor,*this,other);}

    constexpr VToken(VToken&&) noexcept requires (... && std::is_trivially_move_constructible_v<Ts>) = default;
    constexpr VToken(VToken&& other) noexcept :
        Loc(static_cast<Loc&&>(other)),
        tu_(std::in_place_index<0>),
        kind_(other.kind_)
    {yuki::visit_homogenous<void>(yuki::copy_visitor,*this,std::move(other));}

    constexpr ~VToken() noexcept requires (... && std::is_trivially_destructible_v<Ts>) = default;
    constexpr ~VToken() noexcept {yuki::visit_homogenous<void>(yuki::destroy_visitor,*this);}

    constexpr VToken& operator=(const VToken&) noexcept requires (... && std::is_trivially_copy_assignable_v<Ts>) = default;
    constexpr VToken& operator=(const VToken& other) noexcept {
        if(this!=&other){
            Loc::operator=(static_cast<const Loc&>(other));
            if(kind_==other.kind_){
                yuki::visit_homogenous<void>(yuki::assign_visitor,*this,other);
            }else{
                yuki::visit_homogenous<void>(yuki::destroy_visitor,*this);
                kind_=other.kind_;
                yuki::visit_homogenous<void>(yuki::copy_visitor,*this,other);
            }
        }
        return *this;
    }

    constexpr VToken& operator=(VToken&&) noexcept requires (... && std::is_trivially_move_assignable_v<Ts>) = default;
    constexpr VToken& operator=(VToken&& other) noexcept {
        if(this!=&other){
            Loc::operator=(static_cast<Loc&&>(other));
            if(kind_==other.kind_){
                yuki::visit_homogenous<void>(yuki::assign_visitor,*this,std::move(other));
            }else{
                yuki::visit_homogenous<void>(yuki::destroy_visitor,*this);
                kind_=other.kind_;
                yuki::visit_homogenous<void>(yuki::copy_visitor,*this,std::move(other));
            }
        }
        return *this;
    }

    friend constexpr void swap(VToken& lhs,VToken& rhs) noexcept {
        using std::swap;
        swap(static_cast<Loc&>(lhs),static_cast<Loc&>(rhs));
        if(lhs.active_==rhs.active_){
            yuki::visit_homogenous<void>(yuki::swap_visitor,lhs,rhs);
        }else{
            VToken temp = std::move(lhs);
            yuki::visit_homogenous<void>(yuki::destroy_visitor,lhs);
            lhs.kind_=rhs.kind_;
            yuki::visit_homogenous<void>(yuki::copy_visitor,lhs,std::move(rhs));
            yuki::visit_homogenous<void>(yuki::destroy_visitor,rhs);
            rhs.kind_=temp.kind_;
            yuki::visit_homogenous<void>(yuki::copy_visitor,rhs,std::move(temp));
        }
    }

    template<auto K,typename... Args>
    explicit constexpr VToken(yuki::pg::in_place_kind_t<K>,const Loc& loc,Args&&... args) noexcept :
        Loc(loc),
        tu_(std::in_place_index<token_index_table[K]>,std::forward<Args>(args)...),
        kind_(K)
    {}

    template<Token_Kind_t TI>
    constexpr decltype(auto) get() & {assert(TI==token_index_table[kind_]);return tu_.template get<TI>();}
    template<Token_Kind_t TI>
    constexpr decltype(auto) get() const& {assert(TI==token_index_table[kind_]);return tu_.template get<TI>();}
    template<Token_Kind_t TI>
    constexpr decltype(auto) get() && {assert(TI==token_index_table[kind_]);return tu_.template get<TI>();}
    template<Token_Kind_t TI>
    constexpr decltype(auto) get() const&& {assert(TI==token_index_table[kind_]);return tu_.template get<TI>();}
}; // struct VToken
} // namespace yuki::pg
