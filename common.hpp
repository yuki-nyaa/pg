#pragma once
#include<concepts>
#include<string>
#include<yuki/set.hpp>
#include<yuki/set_ov.hpp>

namespace yuki::pg{

typedef unsigned short prec_t;
enum struct Assoc : unsigned char {RIGHT,LEFT};

struct Token_Data{
    std::string name;
    std::string alias;

    prec_t prec=0;
    Assoc assoc;

    yuki::Vector<std::string> types;
    yuki::Vector<std::string> names;

    std::string alloc;

    #ifndef YUKI_PG_TOKEN_DATA_RESERVE
    #define YUKI_PG_TOKEN_DATA_RESERVE 8
    #endif
    Token_Data(std::string&& n,Assoc assoc_p) noexcept :
        name(std::move(n)),
        assoc(assoc_p),
        types(yuki::reserve_tag,YUKI_PG_TOKEN_DATA_RESERVE),
        names(yuki::reserve_tag,YUKI_PG_TOKEN_DATA_RESERVE)
    {}

    std::string& name_or_alias() noexcept {return alias.empty() ? name : alias;}
    const std::string& name_or_alias() const noexcept {return alias.empty() ? name : alias;}
};
} // namespace yuki::pg


namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
struct Rule{
    Token_Kind_t left;
    std::basic_string<Token_Kind_t> rights;

    size_t num;
    prec_t prec_sr=0;
    prec_t prec_rr=0;
    std::string init;
    std::string code;

    friend bool operator==(const Rule& lhs,const Rule& rhs) noexcept {
        return lhs.left==rhs.left && lhs.rights.size()==rhs.rights.size() && lhs.rights==rhs.rights;
    }

    void clear(){
        left = -1;
        rights.clear();
        num = 0;
        prec_sr = 0;
        prec_rr = 0;
        init.clear();
        code.clear();
    }

    bool is_goal() const {return left==(Token_Kind_t)-1;}

    struct Rough_Less;
};

template<std::unsigned_integral Token_Kind_t>
struct Rule<Token_Kind_t>::Rough_Less{
    static std::strong_ordering compare3(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs){
        if(std::strong_ordering cmp=lhs.left<=>rhs.left;cmp!=0) return cmp;
        if(std::strong_ordering cmp=lhs.rights.size()<=>rhs.rights.size();cmp!=0) return cmp;
        if(!lhs.rights.empty())
            if(std::strong_ordering cmp=lhs.rights[0]<=>rhs.rights[0];cmp!=0) return cmp;
        return std::strong_ordering::equal;
    }

    static bool compare(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs) {return compare3(lhs,rhs)==std::strong_ordering::less;}
};

template<std::unsigned_integral Token_Kind_t>
struct Rule_Set : private yuki::MultiSet<Rule<Token_Kind_t>,typename Rule<Token_Kind_t>::Rough_Less> {
  private:
    typedef yuki::MultiSet<Rule<Token_Kind_t>,typename Rule<Token_Kind_t>::Rough_Less> MultiSet_;
  public:
    using typename MultiSet_::key_type;
    using typename MultiSet_::value_type;
    using typename MultiSet_::key_compare;
    using typename MultiSet_::allocator_type;
    using typename MultiSet_::pointer;
    using typename MultiSet_::const_pointer;
    using typename MultiSet_::size_type;
    using typename MultiSet_::difference_type;

    using MultiSet_::MultiSet_;
    friend constexpr void swap(Rule_Set& lhs,Rule_Set& rhs) noexcept {swap(static_cast<MultiSet_&>(lhs),static_cast<MultiSet_&>(rhs));}

    using MultiSet_::clear;
    using MultiSet_::empty;
    using MultiSet_::size;

    using typename MultiSet_::non_const_iterator;
    using typename MultiSet_::const_iterator;

    using MultiSet_::begin;
    const_iterator begin(Token_Kind_t token) const {
        const_iterator i = first_equiv_greater({token,{},0});
        return (i!=end() && i->left==token) ? i : end();
    }
    using MultiSet_::end;
    const_iterator end(Token_Kind_t token) const {return first_equiv_greater({++token,{},0});}

    using MultiSet_::min;
    using MultiSet_::max;

    using MultiSet_::first_greater;
    using MultiSet_::first_equiv_greater;

    const_iterator find(const key_type& k) const {
        const_iterator feg = first_equiv_greater(k);
        const const_iterator fg = first_greater(k);
        for(;feg!=fg;++feg)
            if(*feg==k)
                return feg;
        return end();
    }

    bool contains(const key_type& k) const {
        const_iterator feg = first_equiv_greater(k);
        const const_iterator fg = first_greater(k);
        for(;feg!=fg;++feg)
            if(*feg==k)
                return true;
        return false;
    }

    size_type count(const key_type& k) const {return contains(k) ? 1 : 0;}

    template<typename... Args>
    yuki::IB_Pair<const_iterator> emplace(Args&&... args){
        const const_iterator i = MultiSet_::emplace(std::forward<Args>(args)...);
        for(const_iterator feg = first_equiv_greater(*i);feg!=i;++feg){
            if(*feg==*i){
                erase(feg);
                return {i,false};
            }
        }
        return {i,true};
    }

    // Used for debugging.
    template<typename... Args>
    yuki::IB_Pair<const_iterator> emplace(Token_Kind_t left,std::basic_string<Token_Kind_t> rights,size_t num,Args&&... others){
        const const_iterator i = MultiSet_::emplace(left,std::move(rights),num,std::forward<Args>(others)...);
        for(const_iterator feg = first_equiv_greater(*i);feg!=i;++feg){
            if(*feg==*i){
                erase(feg);
                return {i,false};
            }
        }
        return {i,true};
    }

    using MultiSet_::erase;
};

template<std::unsigned_integral Token_Kind_t>
using First_Set = yuki::Set_OV<Token_Kind_t>;

template<std::unsigned_integral Token_Kind_t>
bool contains_epsilon(First_Set<Token_Kind_t>& fs,std::type_identity_t<Token_Kind_t> e) {return !fs.empty() && fs.tree_base().back()==e;}

template<std::unsigned_integral Token_Kind_t>
void remove_epsilon(First_Set<Token_Kind_t>& fs,std::type_identity_t<Token_Kind_t> e) {if(contains_epsilon(fs,e)) fs.tree_base().pop_back();}

template<std::unsigned_integral Token_Kind_t>
void insert_epsilon(First_Set<Token_Kind_t>& fs,std::type_identity_t<Token_Kind_t> e) {if(!contains_epsilon(fs,e)) fs.tree_base().vec_base().emplace_back(e);}

template<std::unsigned_integral Token_Kind_t>
struct First_Table{
  private:
    yuki::Vector<First_Set<Token_Kind_t>> vec_;
    size_t term_first_;
  public:
	First_Table() = delete;

    First_Table(const size_t nterm_total,const size_t term_total,const Rule_Set<Token_Kind_t>& rules) noexcept :
        term_first_(nterm_total)
    {
        const size_t token_total = term_first_ + term_total;
        vec_.reserve(token_total+1);

        // From "Engineering A Compiler (2nd Edition)" P104
        for(Token_Kind_t i = 0;i<term_first_;++i)
            vec_.force_emplace_back();
        for(Token_Kind_t i = term_first_;i<=token_total;++i)
            vec_.force_emplace_back(yuki::from_ordered_tag,yuki::from_variadic_tag,i);

        {
        bool changed = true;
        while(changed){
            changed=false;
            for(const Rule<Token_Kind_t>& rule : rules){
                if(!rule.is_goal()){
                    const size_t rule_rights_size = rule.rights.size();
                    First_Set<Token_Kind_t> rhs = vec_[rule.rights[0]];
                    remove_epsilon(rhs,token_total);
                    size_t i = 0;
                    while(i+2<=rule_rights_size && contains_epsilon(vec_[rule.rights[i]],token_total)){
                        bool rhs_epsilon = contains_epsilon(rhs,token_total);
                        rhs.merge(vec_[rule.rights[i+1]]);
                        if(!rhs_epsilon)
                            remove_epsilon(rhs,token_total);
                        ++i;
                    }
                    if(i+1==rule_rights_size && contains_epsilon(vec_[rule.rights[i]],token_total))
                        insert_epsilon(rhs,token_total);
                    const size_t size_orig = vec_[rule.left].size();
                    vec_[rule.left].merge(std::move(rhs));
                    if(vec_[rule.left].size()!=size_orig)
                        changed = true;
                }
            }
        }
        }
    }

    First_Table(const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,const Rule_Set<Token_Kind_t>& rules) noexcept :
        First_Table(nterms.size(),terms.size(),rules)
    {}

    size_t term_first() const {return term_first_;}
    size_t eof() const {return vec_.size()-2;}
    size_t epsilon() const {return vec_.size()-1;}

    const First_Set<Token_Kind_t>& operator[](size_t i) const {return vec_[i];}

    struct extended_const_iterator_ne{ // `extended_const_iterator` without epsilon.
      private:
        std::basic_string<Token_Kind_t> tokens;
        const First_Table* ftable;
        typename std::basic_string<Token_Kind_t>::const_iterator x;
        typename First_Set<Token_Kind_t>::const_iterator y;
      public:
        const Token_Kind_t& operator*() const noexcept {return *y;}

        // Since the iterator points to a built-in integral type, no `operator->` is needed.

        bool is_end() const noexcept {return x==tokens.cend();}

        extended_const_iterator_ne& operator++(){
            ++y;
            if(y==(*ftable)[*x].end()){
                x=tokens.cend();
                return *this;
            }
            while(*y==ftable->epsilon()){
                ++x;
                if(!is_end())
                    y = (*ftable)[*x].begin();
                else
                    return *this;
            }
            return *this;
        }

        extended_const_iterator_ne() = delete;

        // This constructor finds the first non-epsilon symbol. If there is none, then the iterator points to epsilon, of course.
        extended_const_iterator_ne(const First_Table* ftable_p, std::basic_string_view<Token_Kind_t> tokens_p, const Token_Kind_t token2_p) noexcept :
            ftable(ftable_p)
        {
            tokens.reserve(tokens_p.size()+1);
            tokens.append(tokens_p.data(),tokens_p.size());
            tokens.push_back(token2_p);
            x=tokens.cbegin();
            y = (*ftable)[*x].begin();

            while(*y==ftable->epsilon()){
                ++x;
                if(!is_end())
                    y = (*ftable)[*x].begin();
                else
                    return;
            }
        }

        // Used for debugging.
        extended_const_iterator_ne(const First_Table* ftable_p, std::initializer_list<Token_Kind_t> tokens_p, const Token_Kind_t token2_p) noexcept :
            extended_const_iterator_ne(ftable_p,std::basic_string_view<Token_Kind_t>(tokens_p.begin(),tokens_p.size()),token2_p)
        {}
    }; // struct extended_const_iterator_ne

    extended_const_iterator_ne begin_ne(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
    extended_const_iterator_ne begin_ne(std::initializer_list<Token_Kind_t> tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
}; // struct First_Table<Token_Kind_t>
} // namespace yuki::pg
