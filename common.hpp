#pragma once
#include<concepts>
#include<vector>
#include<string>
#include<yuki/functional.hpp>
#include<yuki/multiset.hpp>
#include<yuki/print.hpp>

namespace yuki::pg{
typedef unsigned short prec_t;
enum struct Assoc : unsigned char {RIGHT=0,LEFT};

struct Token_Data{
    std::string name;
    std::string alias;

    prec_t prec=0;
    Assoc assoc = Assoc::RIGHT;

    std::vector<std::string> types;
    std::vector<std::string> names;

    std::string alloc;

    Token_Data(std::string&& n) noexcept :
        name(std::move(n))
    {
        #ifndef YUKI_PG_TOKEN_DATA_RESERVE
        #define YUKI_PG_TOKEN_DATA_RESERVE 8
        #endif
        types.reserve(YUKI_PG_TOKEN_DATA_RESERVE);
        names.reserve(YUKI_PG_TOKEN_DATA_RESERVE);
    }

    std::string& name_or_alias() noexcept {return alias.empty() ? name : alias;}
    const std::string& name_or_alias() const noexcept {return alias.empty() ? name : alias;}
};
} // namespace yuki::pg
template<>
struct fmt::formatter<yuki::pg::Assoc,char> : yuki::simple_formatter<yuki::pg::Assoc,char> {
    template<typename OutputIt>
    auto format(yuki::pg::Assoc assoc,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
        switch(assoc){
            case yuki::pg::Assoc::LEFT : {return fmt::format_to(ctx.out(),"{}","LEFT");}
            default : {return fmt::format_to(ctx.out(),"{}","RIGHT");}
        }
    }
};
namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
struct Rule{
    Token_Kind_t left;
    std::basic_string<Token_Kind_t> rights;

    size_t num;
    prec_t prec_sr=0;
    prec_t prec_rr=0;
    std::string init={};
    std::string code={};

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
};
}

namespace yuki{
template<std::unsigned_integral Token_Kind_t>
struct rough_less<pg::Rule<Token_Kind_t>>{
    bool operator()(const pg::Rule<Token_Kind_t>& lhs,const pg::Rule<Token_Kind_t>& rhs) const noexcept {
        if(lhs.left<rhs.left)
            return true;
        else if(rhs.left<lhs.left)
            return false;
        else if(lhs.rights.size()<rhs.rights.size())
            return true;
        else if(rhs.rights.size()<lhs.rights.size())
            return false;
        else if(lhs.rights.size()!=0){
            if(lhs.rights[0]<rhs.rights[0])
                return true;
            else if(lhs.rights[0]>rhs.rights[0])
                return false;
        }
        return false;
    }
};

template<std::unsigned_integral Token_Kind_t>
struct rough_greater<pg::Rule<Token_Kind_t>>{
    bool operator()(const pg::Rule<Token_Kind_t>& lhs,const pg::Rule<Token_Kind_t>& rhs) const noexcept {
        if(lhs.left>rhs.left)
            return true;
        else if(rhs.left>lhs.left)
            return false;
        else if(lhs.rights.size()>rhs.rights.size())
            return true;
        else if(rhs.rights.size()>lhs.rights.size())
            return false;
        else if(lhs.rights.size()!=0){
            if(lhs.rights[0]>rhs.rights[0])
                return true;
            else if(lhs.rights[0]<rhs.rights[0])
                return false;
        }
        return false;
    }
};
}

namespace yuki::pg{
template<std::unsigned_integral Token_Kind_t>
using Rule_Set = yuki::multiset<Rule<Token_Kind_t>,yuki::rough_less<Rule<Token_Kind_t>>>;

template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::iterator begin_with_left(Rule_Set<Token_Kind_t>& rs, const std::type_identity_t<Token_Kind_t> token){
    typename Rule_Set<Token_Kind_t>::iterator i = rs.first_equiv_greater({token,{},0});
    if((*i).left==token)
        return i;
    else
        return rs.end();
}
template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::const_iterator begin_with_left(const Rule_Set<Token_Kind_t>& rs, const std::type_identity_t<Token_Kind_t> token){
    typename Rule_Set<Token_Kind_t>::const_iterator i = rs.first_equiv_greater({token,{},0});
    if((*i).left==token)
        return i;
    else
        return rs.cend();
}
template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::const_iterator cbegin_with_left(const Rule_Set<Token_Kind_t>& rs, const std::type_identity_t<Token_Kind_t> token){
    typename Rule_Set<Token_Kind_t>::const_iterator i = rs.first_equiv_greater({token,{},0});
    if((*i).left==token)
        return i;
    else
        return rs.cend();
}

template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::iterator end_with_left(Rule_Set<Token_Kind_t>& rs, std::type_identity_t<Token_Kind_t> token){return rs.first_equiv_greater({++token,{},0});} // No rule has epsilon as its left hand side, so ++ is safe.
template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::const_iterator end_with_left(const Rule_Set<Token_Kind_t>& rs, std::type_identity_t<Token_Kind_t> token){return rs.first_equiv_greater({++token,{},0});}
template<std::unsigned_integral Token_Kind_t>
typename Rule_Set<Token_Kind_t>::const_iterator cend_with_left(const Rule_Set<Token_Kind_t>& rs, std::type_identity_t<Token_Kind_t> token){return rs.first_equiv_greater({++token,{},0});}



template<std::unsigned_integral Token_Kind_t>
using First_Set = std::set<Token_Kind_t>;

template<std::unsigned_integral Token_Kind_t>
struct First_Table{
  private:
    std::vector<First_Set<Token_Kind_t>> vec_;
  public:
    const size_t term_first;
  private:
    typedef std::vector<First_Set<Token_Kind_t>> table_t;
    typedef typename table_t::iterator iterator;
    typedef typename table_t::reverse_iterator reverse_iterator;
  public:
    typedef First_Set<Token_Kind_t> set_type;
    typedef typename table_t::const_iterator const_iterator;
    typedef typename table_t::const_reverse_iterator const_reverse_iterator;

	First_Table() = delete;

    First_Table(const std::vector<Token_Data>& nterms,const std::vector<Token_Data>& terms,const Rule_Set<Token_Kind_t>& rules) noexcept :
        term_first(nterms.size())
    {
        const size_t term_total = terms.size();
        const size_t token_total = term_first + term_total;
        vec_.reserve(term_first+term_total+1);
        // From "Engineering A Compiler (2nd Edition)" P104
        for(Token_Kind_t i = 0;i<term_first;++i)
            vec_.emplace_back();
        for(Token_Kind_t i = term_first;i<token_total;++i)
            vec_.emplace_back(std::initializer_list<Token_Kind_t>{i});
        vec_.emplace_back(std::initializer_list<Token_Kind_t>{static_cast<Token_Kind_t>(token_total)});
        {
        bool changed = true;
        while(changed==true){
            changed=false;
            for(const Rule<Token_Kind_t>& rule : rules){
                if(rule.left!=(Token_Kind_t)-1){
                    size_t rule_rights_size = rule.rights.size();
                    First_Set<Token_Kind_t> rhs = vec_[rule.rights[0]];
                    rhs.erase(token_total);
                    size_t i = 0;
                    while(i+2<=rule_rights_size && vec_[rule.rights[i]].contains(token_total)){
                        First_Set<Token_Kind_t> set_temp = vec_[rule.rights[i+1]];
                        set_temp.erase(token_total);
                        rhs.merge(std::move(set_temp));
                        ++i;
                    }
                    if(i==rule_rights_size-1 && vec_[rule.rights[i]].contains(token_total)){
                        rhs.insert(token_total);
                    }
                    size_t size_orig = vec_[rule.left].size();
                    vec_[rule.left].merge(std::move(rhs));
                    if(vec_[rule.left].size()!=size_orig){
                        changed = true;
                    }
                }
            }
        }
        }
    }

    size_t token_total() const noexcept {return vec_.size()-1;}

    const_iterator begin() const noexcept {return vec_.begin();}
    const_iterator cbegin() const noexcept {return vec_.cbegin();}

    const_iterator end() const noexcept {return vec_.end();}
    const_iterator cend() const noexcept {return vec_.cend();}

    const_reverse_iterator rbegin() const noexcept {return vec_.rbegin();}
    const_reverse_iterator crbegin() const noexcept {return vec_.crbegin();}

    const_reverse_iterator rend() const noexcept {return vec_.rend();}
    const_reverse_iterator crend() const noexcept {return vec_.crend();}

    const set_type& operator[](Token_Kind_t kind) const {return vec_[kind];}
  private:
    set_type& operator[](Token_Kind_t kind) {return vec_[kind];}
  public:
    struct extended_const_iterator;
    struct extended_const_iterator_ne;

    bool extended_contains(const Token_Kind_t kind, std::basic_string_view<Token_Kind_t> tokens, const Token_Kind_t token2=Token_Kind_t(-1)) const {
        size_t tokens_size = tokens.size();
        bool token2_valid = (token2!=Token_Kind_t(-1));
        assert(!(tokens_size==0 && !token2_valid)); // You shouldn't pass an empty sequence.
        const size_t token_total = vec_.size()-1;
        if(kind==token_total){
            for(size_t i=0;i+1<=tokens_size;++i){
                if(!vec_[tokens[i]].contains(kind))
                    return false;
            }
            if(token2_valid){
                if(!vec_[token2].contains(kind))
                    return false;
            }
            return true;
        }else{
            for(size_t i=0;i+1<=tokens_size;){
                if(vec_[tokens[i]].contains(kind))
                    return true;
                else if(vec_[tokens[i]].contains(token_total))
                    ++i;
                else
                    return false;
            }
            if(token2_valid){
                if(vec_[token2].contains(kind))
                    return true;
            }
            return false;
        }
    }
    bool extended_contains(const Token_Kind_t kind, const std::basic_string<Token_Kind_t>&& tokens, const Token_Kind_t token2=Token_Kind_t(-1)) const {
        return extended_contains(kind,std::basic_string_view<Token_Kind_t>(tokens),token2);
    }

    [[deprecated("Time-consuming")]]
    set_type extended_get(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {
        bool token2_valid = (token2!=Token_Kind_t(-1));
        assert(!(tokens.size()==0 && !token2_valid)); // You shouldn't pass an empty sequence.
        const size_t token_total = vec_.size()-1;
        set_type result{};
        bool all_epsilon = true;
        typename std::basic_string_view<Token_Kind_t>::const_iterator b = tokens.begin();
        typename std::basic_string_view<Token_Kind_t>::const_iterator e = tokens.end();
        for(;b!=e;++b){
            typename set_type::const_iterator s_e = vec_[*b].end();
            result.insert(vec_[*b].begin(),s_e);
            if(*std::prev(s_e)!=token_total){
                result.erase(token_total);
                return result;
            }
        }
        if(token2_valid){
            typename set_type::const_iterator s_e = vec_[token2].end();
            result.insert(vec_[token2].begin(),s_e);
            if(*std::prev(s_e)!=token_total){
                result.erase(token_total);
                return result;
            }
        }
        return result;
    }
    [[deprecated("Time-consuming")]]
    set_type extended_get(const std::basic_string<Token_Kind_t>&& tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {return extended_get(std::basic_string_view<Token_Kind_t>(tokens),token2);}

    extended_const_iterator begin(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {return extended_const_iterator(this,tokens,token2);}
    extended_const_iterator begin(const std::basic_string<Token_Kind_t>&& tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {return extended_const_iterator(this,tokens,token2);}
    extended_const_iterator cbegin(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {return extended_const_iterator(this,tokens,token2);}
    extended_const_iterator cbegin(const std::basic_string<Token_Kind_t>&& tokens,const Token_Kind_t token2=Token_Kind_t(-1)) const {return extended_const_iterator(this,tokens,token2);}

    extended_const_iterator_ne begin_ne(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
    extended_const_iterator_ne begin_ne(const std::basic_string<Token_Kind_t>&& tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
    extended_const_iterator_ne cbegin_ne(std::basic_string_view<Token_Kind_t> tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
    extended_const_iterator_ne cbegin_ne(const std::basic_string<Token_Kind_t>&& tokens,const Token_Kind_t token2) const {return extended_const_iterator_ne(this,tokens,token2);}
}; // struct First_Table

template<std::unsigned_integral Token_Kind_t>
struct First_Table<Token_Kind_t>::extended_const_iterator{ // TODO comments
  private:
    std::basic_string<Token_Kind_t> tokens;
    const First_Table* table_ptr;
    size_t x;
    size_t x_end;
    typename set_type::const_iterator y;
    typename set_type::const_iterator y_last; // The last element, not the pass-the-end.
    bool all_epsilon; // Whether all previous sets contain epsilon, recorded when leaving one set.

    constexpr bool is_plain_advanceable_() const noexcept {return x!=x_end;}
    void plain_advance_(){ // Well, this is not entirely "plain", since some information must be recorded when leaving a set.
        if(y==y_last){
            if(*y!= table_ptr->vec_.size()){
                all_epsilon = false;
                tokens[x_end]=tokens[x]; // Records the last set which contains at least one non-epsilon symbol.
                x=x_end; // Since the current set does not contain epsilon, the iteration halts here.
            }else if((table_ptr->vec_)[tokens[x]].size()>=2){
                tokens[x_end]=tokens[x]; // Records the last set which contains at least one non-epsilon symbol.
                ++x;
            }else{ // In this case, the current set contains only epsilon.
                ++x;
            }
            y=(table_ptr->vec_)[tokens[x]].begin();
            y_last=std::prev((table_ptr->vec_)[tokens[x]].end());
        }else{
            ++y;
        }
    }
  public:
    const Token_Kind_t& operator*() const noexcept {return *y;}
    // Since the iterator points to a built-in integral type, no `operator->` is needed.
    extended_const_iterator& operator++(){
        bool a,b;
        do{
            plain_advance_();
            a = is_plain_advanceable_();
            b = *y==table_ptr->vec_.size();
        }while(a&&b);
        if(!a){
            y=y_last;
            if(!all_epsilon && b)
                --y;
        }
        return *this;
    }
    constexpr bool is_last() const noexcept {return x==x_end;}

    bool increment_if_not_last() {return (x!=x_end)?(this->operator++(),true):false;}

    extended_const_iterator() = delete;
    extended_const_iterator(const First_Table* table_ptr_p, std::basic_string_view<Token_Kind_t> tokens_p, const Token_Kind_t token2_p=Token_Kind_t(-1)): // This constructor finds the first non-epsilon symbol. If there is none, then the iterator points to epsilon, of course.
        table_ptr(table_ptr_p),
        x(0),
        y((table_ptr_p->vec_)[tokens_p[0]].begin()),
        y_last(std::prev((table_ptr_p->vec_)[tokens_p[0]].end())), // The first-set cannot be empty, so this is safe.
        all_epsilon(true)
    {
        bool token2_valid = (token2_p!=Token_Kind_t(-1));
        assert(!(tokens_p.size()==0 && !token2_valid)); // You shouldn't pass an empty sequence.
        x_end = tokens_p.size() + (token2_valid?1:0);
        tokens.reserve(x_end+1); // The extra slot contains the symbol whose first-set contains at least one non-epsilon symbol. If there is none, i.e. all previous symbols' first-set contain a single epsilon, then the first symbol is selected.
        {
        typename std::basic_string_view<Token_Kind_t>::const_iterator b = tokens_p.begin();
        typename std::basic_string_view<Token_Kind_t>::const_iterator e = tokens_p.end();
        for(;b!=e;++b){
            tokens.push_back(*b);
        }
        if(token2_valid)
            tokens.push_back(token2_p);
        tokens.push_back(tokens[0]);
        }

        while(is_plain_advanceable_() && *y==table_ptr->vec_.size()){ // Finds the first non-epsilon symbol.
            plain_advance_();
        }
    }
    extended_const_iterator(const First_Table* table_ptr_p, const std::basic_string<Token_Kind_t>&& tokens_p, const Token_Kind_t token2_p=Token_Kind_t(-1)) : extended_const_iterator(table_ptr_p,std::basic_string_view<Token_Kind_t>(tokens_p),token2_p) {}
}; // struct First_Table<Token_Kind_t>::extended_const_iterator

template<std::unsigned_integral Token_Kind_t>
struct First_Table<Token_Kind_t>::extended_const_iterator_ne{ // `extended_const_iterator` without epsilon.
  private:
    std::basic_string<Token_Kind_t> tokens;
    const First_Table* table_ptr;
    typename std::basic_string<Token_Kind_t>::iterator x;
    typename std::basic_string<Token_Kind_t>::iterator x_end;
    typename set_type::const_iterator y;
    typename set_type::const_iterator y_end;
  public:
    const Token_Kind_t& operator*() const noexcept {return *y;}
    // Since the iterator points to a built-in integral type, no `operator->` is needed.
    bool is_end() const noexcept {return x==x_end;}
    extended_const_iterator_ne& operator++(){
        ++y;
        if(y==y_end){
            x=x_end;
            return *this;
        }
        while(*y== table_ptr->vec_.size()){
            ++x;
            if(x!=x_end){
                y=(table_ptr->vec_)[*x].begin();
                y_end=(table_ptr->vec_)[*x].end();
            }else{
                return *this;
            }
        }
        return *this;
    }

    extended_const_iterator_ne(const First_Table* table_ptr_p, std::basic_string_view<Token_Kind_t> tokens_p, const Token_Kind_t token2_p): // This constructor finds the first non-epsilon symbol. If there is none, then the iterator points to epsilon, of course.
        table_ptr(table_ptr_p)
    {
        tokens.reserve(tokens_p.size()+1);
        tokens.append(tokens_p.data(),tokens_p.size());
        tokens.push_back(token2_p);
        x=tokens.begin();
        x_end=tokens.end();
        y=(table_ptr_p->vec_)[*x].begin();
        y_end=(table_ptr_p->vec_)[*x].end();

        while(*y== table_ptr->vec_.size()){
            ++x;
            if(x!=x_end){
                y=(table_ptr_p->vec_)[*x].begin();
                y_end=(table_ptr_p->vec_)[*x].end();
            }else{
                return;
            }
        }
    }
    extended_const_iterator_ne(const First_Table* table_ptr_p, const std::basic_string<Token_Kind_t>&& tokens_p, const Token_Kind_t token2_p) : extended_const_iterator_ne(table_ptr_p,std::basic_string_view<Token_Kind_t>(tokens_p),token2_p) {}
}; // struct First_Table<Token_Kind_t>::extended_const_iterator_ne
} // namespace yuki::pg

#ifdef YUKI_PG_PRINT
template<std::unsigned_integral Token_Kind_t>
struct fmt::formatter<yuki::pg::First_Set<Token_Kind_t>,char> : yuki::simple_formatter<yuki::pg::First_Set<Token_Kind_t>,char> {
    template<typename OutputIt>
    auto format(const yuki::pg::First_Set<Token_Kind_t>& fs,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
        typename fmt::basic_format_context<OutputIt,char>::iterator o = ctx.out();
        fmt::format_to(o,"(");
        for(const Token_Kind_t kind : fs)
            fmt::format_to(o,"{},",static_cast<unsigned long long>(kind));
        return fmt::format_to(o,")");
    }
};
#endif
