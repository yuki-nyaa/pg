#pragma once
#include<concepts>
#include<string>
#include<yuki/Set.hpp>
#include<yuki/algorithm.hpp>
#include<yuki/Vector.hpp>
#include<yuki/unordered_map_str.hpp>
#include<yuki/uchar.hpp>

namespace yuki::pg{

typedef unsigned short prec_t;
enum struct Assoc : unsigned char {DEFAULT,LEFT,RIGHT,};


struct Token_Data{
    // The following 3 members' order should not be changed.
    std::string name;
    std::string alias;

    std::string alloc;
    //< The preceding 3 members' order should not be changed.
    prec_t prec=0;
    Assoc assoc=Assoc::DEFAULT;

    yuki::Vector<std::string,yuki::Allocator<std::string>,yuki::Default_EC<2,1,1>> types;
    yuki::Vector<std::string,yuki::Allocator<std::string>,yuki::Default_EC<2,1,1>> names;

    size_t vtoken_index = 0;

    std::string& name_or_alias() noexcept {return alias.empty() ? name : alias;}
    const std::string& name_or_alias() const noexcept {return alias.empty() ? name : alias;}
};


struct Token_Datas{
    #ifndef YUKI_PG_NTERMINAL_EXPECTED
    #define YUKI_PG_NTERMINAL_EXPECTED 128
    #endif
    #ifndef YUKI_PG_TERMINAL_EXPECTED
    #define YUKI_PG_TERMINAL_EXPECTED 128
    #endif
    yuki::Vector<Token_Data> nterms{yuki::reserve_tag,YUKI_PG_NTERMINAL_EXPECTED};
    yuki::Vector<Token_Data> terms{yuki::reserve_tag,YUKI_PG_TERMINAL_EXPECTED};
    struct Coordinate{
        bool is_term;
        size_t idx;
    };
    yuki::unordered_map_str<std::string,Coordinate> token_htable{YUKI_PG_NTERMINAL_EXPECTED+YUKI_PG_TERMINAL_EXPECTED};

    size_t goal=0;

    Token_Data& operator[](const Coordinate co) {return (co.is_term ? terms : nterms)[co.idx];}
    const Token_Data& operator[](const Coordinate co) const {return (co.is_term ? terms : nterms)[co.idx];}
    Token_Data& operator[](const std::string& s) {return operator[](token_htable[s]);}

    Coordinate at(const std::string& s) const noexcept(false) {return token_htable.at(s);}

    template<std::unsigned_integral Token_Kind_t>
    const Token_Data& operator[](const Token_Kind_t k) const {return operator[](to_co(k));}

    template<std::unsigned_integral Token_Kind_t>
    Token_Kind_t to_num(const Coordinate co) const {
        return !co.is_term ? co.idx : nterms.size()+co.idx;
    }

    template<std::unsigned_integral Token_Kind_t>
    Coordinate to_co(const Token_Kind_t k) const {
        assert(k<nterms.size()+terms.size());
        return k<nterms.size() ? Coordinate{false,k} : Coordinate{true,k-nterms.size()};
    }

    size_t eof() const {return nterms.size()+terms.size()-1;}

    template<std::unsigned_integral Token_Kind_t>
    bool is_nterm(const Token_Kind_t k) const {return k<nterms.size();}
};


struct Sec0_Data{
    std::string nspace;
    std::string parser = "Parser";
    std::string ts = "Token_Settings";
    std::string lexer = "Lexer";
    std::string parser_tables;

    std::string sp_token;

    size_t lr1_stack_reserve = 128;

    bool no_final_function = false;
    bool no_final_class = false;
    bool no_default_ctor = false;

    bool is_switch = true;

    enum struct Alg_Type : unsigned char {NIL,PLR1,CLR1,LALR1} alg_type = Alg_Type::NIL;

    enum struct Token_Impl_Type : unsigned char {VARIANT,SIMPLE,TUPLE} token_impl_type = Token_Impl_Type::VARIANT;

    bool is_variant() const {return token_impl_type==Token_Impl_Type::VARIANT;}
    bool is_simple() const {return token_impl_type==Token_Impl_Type::SIMPLE;}
    bool is_tuple() const {return token_impl_type==Token_Impl_Type::TUPLE;}

    Assoc assoc0 = Assoc::LEFT;

    unsigned errors=0;

    Token_Datas token_datas;

    struct Code{
        std::string_view qualifier;
        std::string contents;
    };

    Code codes[7]={
        {"class"},
        {"cpp_top"},
        {"h_top"},
        {"token_top"},
        {"SEC2_"},
        {"token_bottom"},
        {"h_bottom"},
    };

    prec_t current_prec=0;

    struct Input{
        size_t lineno_orig=1;
        size_t colno_orig=1;
        size_t lineno=1;
        size_t colno=1;

        yuki::U8Char get(FILE* const in){
            lineno_orig=lineno;
            colno_orig=colno;
            const yuki::U8Char c(in);
            if(c==yuki::U8Char(static_cast<unsigned char>('\n'))){
                ++lineno;
                colno=1;
            }else if(c!=yuki::EOF_U8)
                ++colno;
            return c;
        }

        /// @return `0` if the next byte is not '/'; `EOF` if the next byte is '/' the skipping ends with `EOF`; `(unsigned char)'\n'` if the next byte is '/' the skipping ends with it.
        int try_skip_comment(FILE* const in){
            if(const int peek=fgetc(in); peek==static_cast<unsigned char>('/')){
                while(1){
                    switch(const int peek1=fgetc(in); peek1){
                        case EOF: return EOF;
                        case static_cast<unsigned char>('\r'): fgetc(in); [[fallthrough]];
                        case static_cast<unsigned char>('\n'): ++lineno; colno=1; return static_cast<unsigned char>('\n');
                    }
                }
            }else{
                ungetc(peek,in);
                return 0;
            }
        }
    } input;
}; // Sec0_Data


struct Str_Loc{
    size_t lineno;
    size_t colno;
    std::string str;
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
        return lhs.left==rhs.left && lhs.rights==rhs.rights;
    }

    void clear(){
        rights.clear();
        num = 0;
        prec_sr = 0;
        prec_rr = 0;
    }

    bool is_goal() const {return left==(Token_Kind_t)-1;}

    Token_Kind_t first() const {return rights.empty()? -1 : rights[0];}

    struct Rough_Less;
};

template<std::unsigned_integral Token_Kind_t>
struct Rule<Token_Kind_t>::Rough_Less{
    static std::strong_ordering compare3(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs){
        if(std::strong_ordering cmp=lhs.left<=>rhs.left; cmp!=0) return cmp;
        if(std::strong_ordering cmp=lhs.first()<=>rhs.first(); cmp!=0) return cmp;
        if(std::strong_ordering cmp=lhs.rights.size()<=>rhs.rights.size(); cmp!=0) return cmp;
        return std::strong_ordering::equal;
    }

    static bool operator()(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs) {return compare3(lhs,rhs)<0;}
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
    friend void swap(Rule_Set& lhs,Rule_Set& rhs) noexcept {swap(static_cast<MultiSet_&>(lhs),static_cast<MultiSet_&>(rhs));}

    using MultiSet_::clear;
    using MultiSet_::empty;
    using MultiSet_::size;

    using typename MultiSet_::const_iterator;

    using MultiSet_::begin;
    const_iterator begin(const Token_Kind_t token) const {return first_equiv_greater({token,std::basic_string<Token_Kind_t>(1,0),0});}

    using MultiSet_::end;
    const_iterator end(Token_Kind_t token) const {return first_equiv_greater({++token,std::basic_string<Token_Kind_t>(1,0),0});}

    template<typename F>
    void recursive_traverse(F&& f) const {return MultiSet_::recursive_traverse(std::forward<F>(f));}

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
                MultiSet_::erase(feg);
                return {i,false};
            }
        }
        return {i,true};
    }

    // Used for debugging.
    template<typename... Args>
    yuki::IB_Pair<const_iterator> emplace(const Token_Kind_t left,std::basic_string<Token_Kind_t> rights,const size_t num,Args&&... others){
        const const_iterator i = MultiSet_::emplace(left,std::move(rights),num,std::forward<Args>(others)...);
        for(const_iterator feg = first_equiv_greater(*i);feg!=i;++feg){
            if(*feg==*i){
                MultiSet_::erase(feg);
                return {i,false};
            }
        }
        return {i,true};
    }
};
} // namespace yuki::pg




namespace yuki::pg{
inline constexpr struct dummy_lookahead_t{} dummy_lookahead;

template<std::unsigned_integral Token_Kind_t>
struct Lookaheads{
  private:
    std::basic_string<Token_Kind_t> str;
  public:
    constexpr Lookaheads() noexcept = default;
    constexpr Lookaheads(const Token_Kind_t t) noexcept : str(1,t) {}
    constexpr Lookaheads(dummy_lookahead_t) noexcept : str(1,(Token_Kind_t)-1) {}
    constexpr Lookaheads(const Lookaheads<Token_Kind_t>& other,dummy_lookahead_t) noexcept :
        str(other.str)
    {insert(dummy_lookahead);}

    friend constexpr void swap(Lookaheads& lhs,Lookaheads& rhs) noexcept {swap(lhs.str,rhs.str);}

    constexpr bool empty() const {return str.empty();}
    constexpr size_t size() const {return str.size();}

    void clear() {str.clear();}

    constexpr bool contains(const Token_Kind_t t) const {return yuki::bcontains(str.begin(),str.end(),t);}
    constexpr bool contains(dummy_lookahead_t) const {return !str.empty() && str.back()==(Token_Kind_t)-1;}

    constexpr bool intersects(const Lookaheads<Token_Kind_t>& other) const {
        for(const Token_Kind_t t : other)
            if(contains(t))
                return true;
        return false;
    }

    typedef typename std::basic_string<Token_Kind_t>::const_iterator const_iterator;

    constexpr const_iterator begin() const {return str.begin();}
    /// @note Does NOT include dummy.
    constexpr const_iterator end() const {return contains(dummy_lookahead) ? str.end()-1 : str.end();}

    constexpr Token_Kind_t back() const {assert(!str.empty()); return str.back();}
    constexpr Token_Kind_t operator[](const size_t i) const {assert(i<str.size()); return str[i];}

    bool insert(const Token_Kind_t t){
        const const_iterator pos = yuki::first_equiv_greater(str.begin(),str.end(),t);
        return (pos!=str.end() && *pos==t) ? false : (str.insert(pos,t),true);
    }
    void insert(dummy_lookahead_t) {assert(!contains(dummy_lookahead)); str.push_back(-1);}

    void force_insert(const Token_Kind_t t){
        assert(str.empty() || str.back()<t);
        str.push_back(t);
    }

    void pop_back() {assert(!empty()); str.pop_back();}

    void merge(const Token_Kind_t t) {insert(t);}
    void merge(dummy_lookahead_t) {if(!contains(dummy_lookahead)) insert(dummy_lookahead);}
    void merge(const Lookaheads& other){
        if(str.empty()){
            str = other.str;
        }else{
            for(const Token_Kind_t t : other.str)
                insert(t);
        }
    }
    void merge(Lookaheads&& other){
        if(str.empty()){
            str=std::move(other.str);
        }else{
            if(other.size()<size()){
                using std::swap;
                swap(str,other.str);
            }
            for(const Token_Kind_t t : other.str)
                insert(t);
        }
    }

    friend constexpr bool operator==(const Lookaheads&,const Lookaheads&) noexcept = default;
};

template<std::unsigned_integral Token_Kind_t>
constexpr size_t lookahead_count(Token_Kind_t) {return 1;}
constexpr size_t lookahead_count(dummy_lookahead_t) {return 1;}
template<std::unsigned_integral Token_Kind_t>
size_t lookahead_count(const Lookaheads<Token_Kind_t>& l) {return l.size();}


/// Provides the basic settings for the transitive-closure algorithm described in the famous paper
/// "Efficient Computation of LALR(1) Look-Ahead Sets" by Frank DeRemer and Thomas Pennello (called "algorithm diagraph" in it),
/// which, according to the authors, is an adaptation of the algorithm in the paper
/// "On computing the transitive closure of a relation" by J. Eve and R. Kurki-Suonio.
/// The algorithm used in this program is slightly modified. "tests/tc.cpp" contains the skeleton of the modified algorithm.
template<std::unsigned_integral Token_Kind_t>
struct TC_Context{
  protected:
    const Rule_Set<Token_Kind_t>& rules;
    Token_Kind_t* const depth;
    Token_Kind_t* const stack;
    Token_Kind_t stack_size;
    const Token_Kind_t nterms_size;
  public:
    TC_Context(const size_t nsp,const Rule_Set<Token_Kind_t>& rp) noexcept :
        rules(rp),
        depth(new Token_Kind_t[nsp]{}),
        stack(new Token_Kind_t[nsp]),
        stack_size(0),
        nterms_size(nsp)
    {}

    ~TC_Context() noexcept {delete[] depth; delete[] stack;}

    void reset(){
        assert(stack_size==0);
        memset(depth,0,nterms_size*sizeof(Token_Kind_t));
    }
};


template<std::unsigned_integral Token_Kind_t>
struct Make_FT;

template<std::unsigned_integral Token_Kind_t>
struct First_Table{
    friend Make_FT<Token_Kind_t>;
  private:
    Lookaheads<Token_Kind_t>* first_;
    bool* nullable_;

    First_Table(const Token_Kind_t nterms_size) noexcept :
        first_(new Lookaheads<Token_Kind_t>[nterms_size]),
        nullable_(new bool[nterms_size]{})
    {}
  public:
    void free() const {
        delete[] first_;
        delete[] nullable_;
    }

    const Lookaheads<Token_Kind_t>& operator[](const Token_Kind_t nt) const {return first_[nt];}
    bool nullable(const Token_Kind_t nt) const {return nullable_[nt];}

    /// @return `true` if `l...` are populated, i.e. `rights` is nullable.
    template<typename... L>
    bool populate(Lookaheads<Token_Kind_t>& target,const size_t nterms_size,std::basic_string_view<Token_Kind_t> rights,const L&... l) const {
        while(1){
            if(rights.empty()){
                (...,target.merge(l));
                return true;
            }
            const Token_Kind_t front = rights.front();
            if(front>=nterms_size){
                target.insert(front);
                return false;
            }
            target.merge(first_[front]);
            if(nullable_[front])
                rights.remove_prefix(1);
            else
                return false;
        }
    }
};

template<std::unsigned_integral Token_Kind_t>
struct Make_FT : protected TC_Context<Token_Kind_t>{
  protected:
    const First_Table<Token_Kind_t> first_table;
    using TC_Context<Token_Kind_t>::rules;
    using TC_Context<Token_Kind_t>::depth;
    using TC_Context<Token_Kind_t>::stack;
    using TC_Context<Token_Kind_t>::stack_size;
    using TC_Context<Token_Kind_t>::nterms_size;
  private:
    void traverse(const Token_Kind_t k){
        assert(stack_size<nterms_size);
        stack[stack_size++]=k;
        const Token_Kind_t depth_orig = stack_size;
        depth[k]=depth_orig;

        typename Rule_Set<Token_Kind_t>::const_iterator b = rules.begin(k);
        const typename Rule_Set<Token_Kind_t>::const_iterator e = rules.end(k);

        for(;b!=e && b->first()<nterms_size;++b){
            for(const Token_Kind_t r : b->rights){
                if(r>=nterms_size){
                    first_table.first_[k].insert(r);
                    goto next_rule;
                }else{
                    if(depth[r]==0){
                        traverse(r);
                        first_table.first_[k].merge(first_table.first_[r]);
                    }else if(r!=k)
                        first_table.first_[k].merge(first_table.first_[r]);
                    if(depth[r] < depth[k])
                        depth[k] = depth[r];
                    if(!first_table.nullable_[r])
                        goto next_rule;
                }
            }
            first_table.nullable_[k]=true;
            next_rule:;
        }

        for(;b!=e;++b){
            const Token_Kind_t rule_first = b->first();
            if(rule_first==(Token_Kind_t)-1){
                first_table.nullable_[k]=true;
                break;
            }
            first_table.first_[k].insert(rule_first);
        }

        if(depth[k]==depth_orig){
            while(1){
                const Token_Kind_t pop = stack[--stack_size];
                depth[pop] = -1;
                if(pop==k)
                    return;
                else
                    first_table.first_[pop]=first_table.first_[k];
            }
        }
    }
  public:
    Make_FT(const size_t nsp,const Rule_Set<Token_Kind_t>& rp) noexcept :
        TC_Context<Token_Kind_t>(nsp,rp),
        first_table(nsp)
    {}

    First_Table<Token_Kind_t> calc(){
        traverse(0);
        for(Token_Kind_t i=1;i<nterms_size;++i)
            if(depth[i]==0)
                traverse(i);
        return first_table;
    }
};
} // namespace yuki::pg
