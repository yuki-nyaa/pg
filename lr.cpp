#include"cconfig"
#include"common.hpp"
#include"cmd.hpp"
#include<yuki/Set.hpp>
#include<yuki/Map.hpp>
#include<yuki/Set_OV.hpp>
#include<yuki/Map_OV.hpp>
#include<yuki/RingQueue.hpp>
#include<yuki/print.hpp>
#include<cstdio>

namespace yuki::pg::lr{

template<std::unsigned_integral Token_Kind_t>
constexpr Token_Kind_t get_cursored(const std::basic_string_view<Token_Kind_t> rights,const size_t cursor){
    return cursor<rights.size() ? rights[cursor] : (Token_Kind_t)-1;
}

template<std::unsigned_integral Token_Kind_t>
constexpr Token_Kind_t get_accessing_symbol(const std::basic_string_view<Token_Kind_t> rights,const size_t cursor){
    assert(!rights.empty() && cursor>0);
    return rights[cursor-1];
}

template<std::unsigned_integral Token_Kind_t>
constexpr std::basic_string_view<Token_Kind_t> get_tail_string(std::basic_string_view<Token_Kind_t> rights,const size_t cursor){
    rights.remove_prefix(cursor+1);
    return rights;
}

template<std::unsigned_integral Token_Kind_t>
struct Core{
    std::basic_string_view<Token_Kind_t> rights;
    size_t cursor;

    friend constexpr std::strong_ordering operator<=>(const Core lhs,const Core rhs) noexcept {
        if(std::strong_ordering cmp=get_cursored(lhs.rights,lhs.cursor)<=>get_cursored(rhs.rights,rhs.cursor); cmp!=0) return cmp;
        if(std::strong_ordering cmp=yuki::compare3(lhs.rights.data(),rhs.rights.data()); cmp!=0) return cmp;
        if(std::strong_ordering cmp=lhs.cursor<=>rhs.cursor; cmp!=0) return cmp;
        return std::strong_ordering::equal;
    }
};

template<typename Item>
concept has_lookaheads = requires(Item item){item.lookaheads;};

static_assert(!has_lookaheads<Core<unsigned char>>);

struct To_Core{
    template<typename Item>
    static constexpr Core<typename Item::Token_Kind_t> operator()(const Item& item){
        return {item.rule.rights,item.cursor};
    }
    typedef void is_transparent;
};

constexpr const char log_banner[] = "================================================================"; // 64

template<std::unsigned_integral Token_Kind_t>
void print(FILE* const out,const Token_Datas& token_datas,const Core<Token_Kind_t> core){
    fputs("{# -> ",out);
    for(size_t i=0;i<core.rights.size();++i)
        fprintf(out,"%s%s ",i==core.cursor?". ":"",token_datas[core.rights[i]].name_or_alias().c_str());
    fprintf(out,"%s}",core.cursor>=core.rights.size()?".":"");
}

template<typename Item> requires (!has_lookaheads<Item>)
void print(FILE* const out,const Token_Datas& token_datas,const Item item){
    fprintf(out,"{%s -> ",token_datas[item.rule.left].name_or_alias().c_str());
    for(size_t i=0;i<item.rule.rights.size();++i)
        fprintf(out,"%s%s ",i==item.cursor?". ":"",token_datas[item.rule.rights[i]].name_or_alias().c_str());
    fprintf(out,"%s}",item.cursor>=item.rule.rights.size()?".":"");
}

template<has_lookaheads Item>
void print(FILE* const out,const Token_Datas& token_datas,const Item& item){
    fprintf(out,"{%s -> ",token_datas[item.rule.left].name_or_alias().c_str());
    const std::basic_string_view<typename Item::Token_Kind_t> rights = item.rule.rights;
    const size_t cursor = item.cursor;
    for(size_t i=0;i<rights.size();++i)
        fprintf(out,"%s%s ",i==cursor?". ":"",token_datas[rights[i]].name_or_alias().c_str());
    fprintf(out,"%s {",cursor>=rights.size()?".":"");
    for(const typename Item::Token_Kind_t lh : item.lookaheads)
        fprintf(out,"%s ",token_datas[lh].name_or_alias().c_str());
    if(item.lookaheads.contains(dummy_lookahead))
        fputc(static_cast<unsigned char>('#'),out);
    fputs("}}",out);
}

template<std::unsigned_integral Token_Kind_t>
void print(FILE* const out,const Token_Datas& token_datas,const First_Table<Token_Kind_t> ft){
    const Token_Kind_t nterms_size = token_datas.nterms.size();
    for(Token_Kind_t i=0;i<nterms_size;++i){
        fprintf(out,"%s\n",token_datas[i].name_or_alias().c_str());
        for(const Token_Kind_t l : ft[i])
            fprintf(out,"%s ",token_datas[l].name_or_alias().c_str());
        if(ft.nullable(i))
            fputs("%empty",out);
        fputs("\n\n",out);
    }
}




inline constexpr struct make_goal_rule_view_tag_t{} make_goal_rule_view_tag;

template<std::unsigned_integral Token_Kind_t>
struct Rule_View{
    std::basic_string_view<Token_Kind_t> rights;
    size_t num;
    prec_t prec_sr;
    prec_t prec_rr;
    Token_Kind_t left;

    constexpr Rule_View(make_goal_rule_view_tag_t) noexcept :
        rights(),
        num(0),
        prec_sr(-1),
        prec_rr(-1),
        left(-1)
    {}

    constexpr Rule_View(const Rule<Token_Kind_t>& rule) noexcept :
        rights(rule.rights),
        num(rule.num),
        prec_sr(rule.prec_sr),
        prec_rr(rule.prec_rr),
        left(rule.left)
    {}
};




struct One_Size{
  private:
    size_t s=0;
  public:
    constexpr bool empty() const {return s==0;}
    constexpr size_t size() const {return s;}
    constexpr void clear() {s=0;}
    constexpr void advance(const size_t sp) {s+=sp;}
};

struct No_Size{
    constexpr No_Size() noexcept = default;
    constexpr No_Size(One_Size) noexcept {}

    static constexpr bool empty() {return true;}
    static constexpr size_t size() {return 0;}
    static constexpr void clear() {}
    static constexpr void advance(const size_t) {}
};


template<typename Size>
struct With_Size : private Size{
  private:
    size_t c=0;
  public:
    constexpr With_Size() noexcept = default;
    constexpr With_Size(const size_t cp) noexcept : Size(), c(cp) {}
    constexpr With_Size(const size_t cp,const Size sp) noexcept : Size(sp), c(cp) {}
    friend With_Size<No_Size>;
    constexpr With_Size(const With_Size<One_Size>& with_one_size) noexcept : Size(static_cast<One_Size>(with_one_size)), c(with_one_size.c) {}

    constexpr bool empty() const {return Size::empty() && c==0;}
    constexpr size_t core_count() const {return c;}
    constexpr size_t size() const {return Size::size();}
    constexpr void clear() {Size::clear();c=0;}
    constexpr void advance(const size_t cp,const size_t sp) {Size::advance(sp);c+=cp;}
};

template<typename Item,typename Total_Size>
struct Flat_Item_Set;

template<typename Item,typename Total_Size>
struct Item_Set : private Total_Size{
    typedef typename Item::Token_Kind_t Token_Kind_t;
    typedef Item item_type;
    typedef Total_Size total_size_type;
    typedef yuki::GSet<Core<Token_Kind_t>,Item,To_Core> set_type;

    friend struct Flat_Item_Set<Item,Total_Size>;
  private:
    set_type set_;
    With_Size<Total_Size> reductions_size_;
  public:
    Item_Set() noexcept = default;
    Item_Set(const Item_Set&) noexcept = default;

    template<typename Total_Size2>
    Item_Set(Item_Set<Item,Total_Size2>&& other) noexcept :
        Total_Size(static_cast<Total_Size2>(other)),
        set_(std::move(other.set_)),
        reductions_size_(other.reductions_size_)
    {other.Total_Size2::clear(); other.reductions_size_.clear();}

    Item_Set(Item_Set<Item,No_Size>&& other) noexcept requires std::is_same_v<Total_Size,One_Size> :
        Total_Size(),
        set_(std::move(other.set_)),
        reductions_size_(other.reductions_size_.core_count(),Total_Size())
    {
        other.No_Size::clear();
        other.reductions_size_.clear();
        struct LK_Counter{
            size_t lk_count=0;
            size_t reduction_lk_count=0;

            void operator()(const Item& item){
                lk_count+=item.lookaheads.size();
                if(item.cursor>=item.rule.rights.size())
                    reduction_lk_count+=item.lookaheads.size();
            }
        } lk_counter;
        set_.tree().recursive_traverse(lk_counter);
        Total_Size::advance(lk_counter.lk_count);
        reductions_size_.advance(0,lk_counter.reduction_lk_count);
    }

    Item_Set& operator=(const Item_Set&) noexcept = default;
    Item_Set& operator=(Item_Set&& other) noexcept = delete;

    typedef typename set_type::const_iterator const_iterator;

    const_iterator begin() const {return set_.begin();}
    const_iterator end() const {return set_.end();}

    bool empty() const {return set_.empty();}
    size_t core_count() const {return set_.size();}
    size_t total_size() const {return Total_Size::size();}
    size_t reductions_core_count() const {return reductions_size_.core_count();}
    size_t reductions_total_size() const {return reductions_size_.size();}

    Token_Kind_t accessing_symbol() const {assert(!set_.empty()); return get_accessing_symbol(set_.tree().root()->rule.rights,set_.tree().root()->cursor);}

    void clear() {Total_Size::clear(); set_.clear(); reductions_size_.clear(); /*traits_.clear();*/}

    yuki::IB_Pair<const_iterator> emplace(const Rule_View<Token_Kind_t> rule,const Token_Kind_t c){
        assert(set_.empty() || rule.num==0 || get_accessing_symbol(rule.rights,c)==accessing_symbol());
        const yuki::IB_Pair<const_iterator> result = set_.emplace_at({rule.rights,c},rule,c);
        if(result.has_inserted){
            Total_Size::advance(1);
            if(c>=rule.rights.size())
                reductions_size_.advance(1,1);
        }
        return result;
    }

    template<typename... L>
    yuki::IB_Pair<const_iterator> emplace(const Rule_View<Token_Kind_t> rule,const Token_Kind_t c,L&&... lookaheads){
        assert(set_.empty() || rule.num==0 || get_accessing_symbol(rule.rights,c)==accessing_symbol());
        if constexpr(has_lookaheads<Item>){
            auto lookahead_count_this = [](const L&... args)->size_t {
                if constexpr(std::is_same_v<Total_Size,One_Size>)
                    return (...+lookahead_count(args));
                else
                    return 0;
            };
            const size_t lc = lookahead_count_this(std::forward<L>(lookaheads)...);
            const yuki::IB_Pair<const_iterator> result = set_.emplace_at({rule.rights,c},rule,c,std::forward<L>(lookaheads)...);
            if(result.has_inserted){
                Total_Size::advance(lc);
                if(c>=rule.rights.size())
                    reductions_size_.advance(1,lc);
            }else{
                const size_t lk_size_orig = result.iterator->lookaheads.size();
                (...,yuki::as_non_const(result.iterator->lookaheads).merge(std::forward<L>(lookaheads)));
                Total_Size::advance(result.iterator->lookaheads.size()-lk_size_orig);
                if(c>=rule.rights.size())
                    reductions_size_.advance(0,result.iterator->lookaheads.size()-lk_size_orig);
            }
            return result;
        }else
            return emplace(rule,c);
    }

    template<typename IS>
    void merge(IS&& src){
        typedef typename std::remove_cvref_t<IS>::item_type Item2;
        static_assert(std::is_same_v<Token_Kind_t,typename Item2::Token_Kind_t>);
        assert(set_.empty() || src.empty() || src.accessing_symbol()==accessing_symbol());
        auto merger = [this](yuki::forward_like_type<IS&&,Item2> src1){
            struct Aux{
                static yuki::IB_Pair<const_iterator> insert(set_type& set,yuki::forward_like_type<IS&&,Item2> src2){
                    if constexpr(std::is_same_v<Item,Item2>)
                        return set.insert(yuki::forward_like<IS>(src2));
                    else if constexpr(has_lookaheads<Item> && has_lookaheads<Item2>)
                        return set.emplace_at({src2.rule.rights,src2.cursor},src2.rule,src2.cursor,yuki::forward_like<IS>(src2.lookaheads));
                    else
                        return set.emplace_at({src2.rule.rights,src2.cursor},src2.rule,src2.cursor);
                }
            };
            if constexpr(has_lookaheads<Item2>){
                const size_t src1_lk_size_orig = src1.lookaheads.size();
                const yuki::IB_Pair<const_iterator> inserted = Aux::insert(set_,yuki::forward_like<IS>(src1));
                if(inserted.has_inserted){
                    Total_Size::advance(src1_lk_size_orig);
                    if(src1.cursor>=src1.rule.rights.size())
                        reductions_size_.advance(1,src1_lk_size_orig);
                }else{
                    const size_t lk_size_orig = inserted.iterator->lookaheads.size();
                    yuki::as_non_const(inserted.iterator->lookaheads).merge(yuki::forward_like<IS>(src1.lookaheads));
                    Total_Size::advance(inserted.iterator->lookaheads.size()-lk_size_orig);
                    if(src1.cursor>=src1.rule.rights.size())
                        reductions_size_.advance(0,inserted.iterator->lookaheads.size()-lk_size_orig);
                }
            }else{
                const yuki::IB_Pair<const_iterator> inserted = Aux::insert(set_,yuki::forward_like<IS>(src1));
                if(src1.cursor>=src1.rule.rights.size())
                    reductions_size_.advance(inserted.has_inserted?1:0,0);
            }
        };
        if constexpr(std::is_const_v<std::remove_reference_t<IS>>)
            src.set_.tree().recursive_traverse(merger);
        else
            src.set_.tree().recursive_popfront(merger);
    }

    template<typename Item2,typename Total_Size2>
    friend struct Item_Set;

    template<typename Item2,typename Total_Size2,typename L0,typename... Ls>
    Item_Set(const Item_Set<Item2,Total_Size2>& other,const L0& l0,const Ls&... ls) noexcept :
        Total_Size(),
        set_(),
        reductions_size_(other.reductions_size_.core_count())
    {
        static_assert(has_lookaheads<Item> && has_lookaheads<Item2>);
        auto f = [this,&l0,&ls...](const Item2& src)->Item{
            Item ret = {src.rule,src.cursor,src.lookaheads};
            if(ret.lookaheads.contains(dummy_lookahead)){
                ret.lookaheads.pop_back();
                ret.lookaheads.merge(l0);
                (...,ret.lookaheads.merge(ls));
                Total_Size::advance(ret.lookaheads.size());
                if(ret.cursor>=ret.rule.rights.size())
                    reductions_size_.advance(0,ret.lookaheads.size());
            }
            return ret;
        };
        set_ .assign(yuki::from_ordered_tag,other.set_.tree().template isomorphic_copy<typename set_type::tree_type>(f));
    }

    void substitute_dummy(const Lookaheads<Token_Kind_t>& lookaheads,dummy_lookahead_t){
        if constexpr(has_lookaheads<Item>){
            auto f = [this,&lookaheads](Item& item){
                if(item.lookaheads.contains(dummy_lookahead)){
                    const size_t size_orig = item.lookaheads.size();
                    item.lookaheads.pop_back();
                    item.lookaheads.merge(lookaheads);
                    item.lookaheads.insert(dummy_lookahead);
                    const size_t merged = item.lookaheads.size()-size_orig;
                    Total_Size::advance(merged);
                    if(item.cursor>=item.rule.rights.size())
                        reductions_size_.advance(0,merged);
                }
            };
            set_.tree().recursive_traverse(f);
        }
    }
}; // struct Item_Set




template<std::unsigned_integral Token_Kind_t_p>
struct LR0_IT_Item{
    typedef Token_Kind_t_p Token_Kind_t;

    Rule_View<Token_Kind_t> rule;
    static constexpr size_t cursor=1;

    template<typename... Args>
    constexpr LR0_IT_Item(const Rule_View<Token_Kind_t> rp,const Args&...) noexcept : rule(rp) {}
};

template<std::unsigned_integral Token_Kind_t>
using LR0_IT_Item_Set = Item_Set<LR0_IT_Item<Token_Kind_t>,No_Size>;

struct Get_Accessing_Symbol{
    template<typename IS>
    static auto operator()(const IS& item_set) {return item_set.accessing_symbol();}
};

template<std::unsigned_integral Token_Kind_t>
using LR0_ImmTransitions = yuki::GSet<Token_Kind_t,LR0_IT_Item_Set<Token_Kind_t>,Get_Accessing_Symbol>;

template<std::unsigned_integral Token_Kind_t>
void print(FILE* const out,const Token_Datas& token_datas,const LR0_ImmTransitions<Token_Kind_t>* const its){
    const Token_Kind_t nterms_size = token_datas.nterms.size();
    for(Token_Kind_t i=0;i<nterms_size;++i){
        fprintf(out,"%s\n%s\n%s\n",log_banner,token_datas[i].name_or_alias().c_str(),log_banner);
        for(const LR0_IT_Item_Set<Token_Kind_t> item_set : its[i]){
            fprintf(out,"%s    %zu %zu (%zu %zu)\n",token_datas[item_set.accessing_symbol()].name_or_alias().c_str(),item_set.core_count(),item_set.total_size(),item_set.reductions_core_count(),item_set.reductions_total_size());
            for(const LR0_IT_Item<Token_Kind_t> item : item_set){
                print(out,token_datas,item);
                fputc(static_cast<unsigned char>('\n'),out);
            }
        }
    }
}

template<std::unsigned_integral Token_Kind_t>
struct Make_LR0_ITs : private TC_Context<Token_Kind_t>{
  private:
    LR0_ImmTransitions<Token_Kind_t>* const immediate_transitions;
    using TC_Context<Token_Kind_t>::rules;
    using TC_Context<Token_Kind_t>::depth;
    using TC_Context<Token_Kind_t>::stack;
    using TC_Context<Token_Kind_t>::stack_size;
    using TC_Context<Token_Kind_t>::nterms_size;

    static void merge_item_set(LR0_IT_Item_Set<Token_Kind_t>& dst,const LR0_IT_Item_Set<Token_Kind_t>& src){
        dst.merge(src);
    }

    void traverse(const Token_Kind_t k){
        assert(stack_size<nterms_size);
        stack[stack_size++]=k;
        const Token_Kind_t depth_orig = stack_size;
        depth[k]=depth_orig;

        typename Rule_Set<Token_Kind_t>::const_iterator b = rules.begin(k);
        const typename Rule_Set<Token_Kind_t>::const_iterator e = rules.end(k);
        for(;b!=e;++b){
            const Token_Kind_t rule_first = b->first();
            if(rule_first>=nterms_size)
                break;
            if(depth[rule_first]==0){
                traverse(rule_first);
                immediate_transitions[k].merge(immediate_transitions[rule_first],merge_item_set);
            }else if(rule_first!=k){
                immediate_transitions[k].merge(immediate_transitions[rule_first],merge_item_set);
            }
            if(depth[rule_first] < depth[k])
                depth[k] = depth[rule_first];
            yuki::as_non_const(*(immediate_transitions[k].emplace_at(rule_first).iterator)).emplace(*b,1);
        }

        for(;b!=e;++b){
            const Token_Kind_t rule_first = b->first();
            if(rule_first==(Token_Kind_t)-1)
                break;
            yuki::as_non_const(*(immediate_transitions[k].emplace_at(rule_first).iterator)).emplace(*b,1);
        }

        if(depth[k]==depth_orig){
            while(1){
                const Token_Kind_t pop = stack[--stack_size];
                depth[pop] = -1;
                if(pop==k)
                    return;
                else
                    immediate_transitions[pop]=immediate_transitions[k];
            }
        }
    }
  public:
    Make_LR0_ITs(const size_t nsp,const Rule_Set<Token_Kind_t>& rp) noexcept :
        TC_Context<Token_Kind_t>(nsp,rp),
        immediate_transitions(new LR0_ImmTransitions<Token_Kind_t>[nsp])
    {}

    const LR0_ImmTransitions<Token_Kind_t>* calc(){
        traverse(0);
        for(Token_Kind_t i=1;i<nterms_size;++i)
            if(depth[i]==0)
                traverse(i);
        return immediate_transitions;
    }
};




template<std::unsigned_integral Token_Kind_t>
struct Reduction{
    Token_Kind_t term;
    Token_Kind_t left; ///< For printing the log only.
    prec_t prec_sr;
    prec_t prec_rr;
    size_t rule;

    template<typename R>
    Reduction(const Token_Kind_t tp,const R& rp) noexcept :
        term(tp),
        left(rp.left),
        prec_sr(rp.prec_sr),
        prec_rr(rp.prec_rr),
        rule(rp.num)
    {}

    constexpr bool valid() const {return rule!=0 || prec_rr!=0;}
    constexpr void make_invalid() {prec_sr=0;prec_rr=0;rule=0;}

    struct Term{
        static constexpr Token_Kind_t operator()(const Reduction r) {return r.term;}
    };
};

struct RR_Conflict_Resolver{
    FILE* out;
    const Token_Datas& token_datas;
    size_t& rr_conflicts;
    size_t state;
    size_t dst;
    size_t src=0;
    const char* occasion_str="";

    // current_actions.merge({fp_log,token_datas,rr_conflicts,current_state->index,0,current_cursored},lr1_ias[current_cursored],lookaheads_pending); state!=0 os=0 src!=0

    // immediate_actions[k].empty_reductions.insert({fp_log,token_datas,rr_conflicts,0,k},{(Token_Kind_t)-1,b->left,b->prec_sr,b->prec_rr,b->num}); state=0 os=0 src=0

    // immediate_actions[k].merge({fp_log,token_datas,rr_conflicts,0,k,rule_first,"Merging empty-reductions"},immediate_actions[rule_first],lookaheads_temp); state=0 os!=0 src!=0

    template<std::unsigned_integral Token_Kind_t>
    void operator()(Reduction<Token_Kind_t>& current,const Reduction<Token_Kind_t> incoming) const {
        assert(current.term==incoming.term);
        if(current.rule==incoming.rule)
            return;
        ++rr_conflicts;
        const bool result = current.prec_rr==incoming.prec_rr
            ? current.rule>incoming.rule
            : current.prec_rr<incoming.prec_rr;
        if(out){
            fputs("RR-conflict in ",out);
            if(state)
                fprintf(out,"state %zu at - %s -. ",state, current.term==(Token_Kind_t)-1 ? "#" : token_datas[current.term].name_or_alias().c_str());
            else
                fprintf(out,"%s at - %s -. ",token_datas[dst].name_or_alias().c_str(), current.term==(Token_Kind_t)-1 ? "#" : token_datas[current.term].name_or_alias().c_str());
            if(*occasion_str!='\0'){
                fprintf(out,"(%s",occasion_str);
                if(src!=(size_t)-1)
                    fprintf(out," from - %s -.) ",token_datas[src].name_or_alias().c_str());
                else
                    fputs(".) ",out);
            }else if(src!=(size_t)-1)
                fprintf(out,"(From - %s -.) ",token_datas[src].name_or_alias().c_str());
            fprintf(out,"(rule=%zu(%s),prec_rr=%u) (rule=%zu(%s),prec_rr=%u). Resolved to %s(%zu).\n",
                current.rule,
                current.left==(Token_Kind_t)-1 ? "Goal_" : token_datas[current.left].name_or_alias().c_str(),
                unsigned(current.prec_rr),
                incoming.rule,
                incoming.left==(Token_Kind_t)-1 ? "Goal_" : token_datas[incoming.left].name_or_alias().c_str(),
                unsigned(incoming.prec_rr),
                result ? "INCOMING" : "CURRENT",
                result ? incoming.rule : current.rule
            );
        }
        if(result)
            current=incoming;
    }
};

struct RS_Conflict_Resolver{
    FILE* out;
    const Token_Datas& token_datas;
    size_t& rs_conflicts;
    size_t state;

    /// @pre `current.term` should be equal to the token number of `shift_token`.
    template<std::unsigned_integral Token_Kind_t>
    bool operator()(Reduction<Token_Kind_t>& current,const Token_Data& shift_token) const {
        ++rs_conflicts;
        const bool result = current.prec_sr==shift_token.prec
            ? shift_token.assoc==Assoc::RIGHT
            : current.prec_sr<shift_token.prec;
        if(out){
            fprintf(out,
                "RS-conflict at state %zu at - %s -. (rule=%zu(%s),prec_sr=%u) (prec=%u,assoc=%s). Resolved to %s.\n",
                state,shift_token.name_or_alias().c_str(),
                current.rule,
                current.left==(Token_Kind_t)-1 ? "Goal_" : token_datas[current.left].name_or_alias().c_str(),
                current.prec_sr,
                shift_token.prec,shift_token.assoc==Assoc::LEFT?"LEFT":"RIGHT",
                result ? "SHIFT" : "REDUCE"
            );
        }
        if(result)
            current.make_invalid();
        return result;
    }
};

template<std::unsigned_integral Token_Kind_t>
struct Reduction_Set : private yuki::GSet_OV<Token_Kind_t,Reduction<Token_Kind_t>,typename Reduction<Token_Kind_t>::Term>{
  private:
    typedef yuki::GSet_OV<Token_Kind_t,Reduction<Token_Kind_t>,typename Reduction<Token_Kind_t>::Term> GSet_;
  public:
    using GSet_::empty;
    using GSet_::size;
    using GSet_::clear;

    bool contains(dummy_lookahead_t) const {return !empty() && GSet_::max()->term==(Token_Kind_t)-1;}

    Reduction<Token_Kind_t> dummy() const {assert(contains(dummy_lookahead)); return *(GSet_::max());}

    typedef typename GSet_::const_iterator const_iterator;

    using GSet_::begin;
    const_iterator end() const {return contains(dummy_lookahead) ? GSet_::end()-1 : GSet_::end();}

    void reset() {GSet_::tree_type::reset();}

    void insert(const RR_Conflict_Resolver& rrcr,const Reduction<Token_Kind_t> reduction){
        const yuki::IB_Pair<const_iterator> ibp = GSet_::insert(reduction);
        if(!ibp.has_inserted)
            rrcr(yuki::as_non_const(*ibp.iterator),reduction);
    }

    void expand_dummy(const RR_Conflict_Resolver& rrcr,Reduction<Token_Kind_t> dummy_reduction,const Token_Kind_t k){
        dummy_reduction.term=k;
        insert(rrcr,dummy_reduction);
    }

    void expand_dummy(const RR_Conflict_Resolver& rrcr,Reduction<Token_Kind_t> dummy_reduction,dummy_lookahead_t){
        const const_iterator max = GSet_::max();
        if(max!=GSet_::end() && max->term==(Token_Kind_t)-1){
            dummy_reduction.term=(Token_Kind_t)-1;
            rrcr(yuki::as_non_const(*max),dummy_reduction);
        }else
            GSet_::tree().vector().emplace_back(dummy_reduction);
    }

    void expand_dummy(const RR_Conflict_Resolver& rrcr,const Reduction<Token_Kind_t> dummy_reduction,const Lookaheads<Token_Kind_t>& l){
        for(const Token_Kind_t t : l)
            expand_dummy(rrcr,dummy_reduction,t);
        if(l.contains(dummy_lookahead))
            expand_dummy(rrcr,dummy_reduction,dummy_lookahead);
    }

    template<typename... L>
    void merge(RR_Conflict_Resolver rrcr,const Reduction_Set& src,const L&... l){
        if(empty()){
            operator=(src);
            if(contains(dummy_lookahead)){
                const Reduction<Token_Kind_t> dummy_reduction = *(GSet_::max());
                GSet_::tree().pop_back();
                rrcr.occasion_str="Expanding dummy-empty-reduction";
                (...,expand_dummy(rrcr,dummy_reduction,l));
            }
            return;
        }
        for(const Reduction<Token_Kind_t> reduction : src)
            insert(rrcr,reduction);
        if(src.contains(dummy_lookahead)){
            rrcr.occasion_str="Expanding dummy-empty-reduction";
            (...,expand_dummy(rrcr,*(src.GSet_::max()),l));
        }
    }

    /// @return `true` for shift.
    bool resolve_rs_conflict(const RS_Conflict_Resolver rscr,const Token_Kind_t tk,const Token_Data& td){
        const const_iterator feg = GSet_::first_equiv_greater(tk);
        return (feg!=GSet_::end() && feg->term==tk) ? rscr(yuki::as_non_const(*feg),td) : true;
    }
};




template<std::unsigned_integral Token_Kind_t>
struct Lookaheads_Pair{
    Lookaheads<Token_Kind_t> lookaheads;
    Lookaheads<Token_Kind_t> lookaheads_pending;

    template<typename... Args>
    constexpr Lookaheads_Pair(Args&&... args) noexcept : lookaheads(std::forward<Args>(args)...) {}
};

template<std::unsigned_integral Token_Kind_t>
struct Lookahead_PropList{
  private:
    struct Order{
        static constexpr std::strong_ordering compare3(const std::basic_string_view<Token_Kind_t> lhs,const std::basic_string_view<Token_Kind_t> rhs){
            assert(!lhs.empty() && !rhs.empty());
            if(std::strong_ordering cmp=(lhs.front())<=>(rhs.front()); cmp!=0) return cmp;
            if(std::strong_ordering cmp=yuki::compare3(lhs.data(),rhs.data()); cmp!=0) return cmp;
            return std::strong_ordering::equal;
        }
        static constexpr bool operator()(const std::basic_string_view<Token_Kind_t> lhs,const std::basic_string_view<Token_Kind_t> rhs) {return compare3(lhs,rhs)<0;}
    };
    Lookaheads<Token_Kind_t> accessing_symbols_;
    yuki::Set_OV<std::basic_string_view<Token_Kind_t>,Order> set_;
  public:
    const Lookaheads<Token_Kind_t>& accessing_symbols() const {return accessing_symbols_;}

    auto size() const {return set_.size();}

    void insert(const std::basic_string_view<Token_Kind_t> rights){
        assert(!rights.empty());
        accessing_symbols_.insert(rights.front());
        set_.insert(rights);
    }

    void merge(const Lookahead_PropList& src){
        accessing_symbols_.merge(src.accessing_symbols_);
        set_.merge(src.set_);
    }

    friend void print(FILE* const out,const Token_Datas& token_datas,const Lookahead_PropList<Token_Kind_t>& lpl){
        for(const Token_Kind_t as : lpl.accessing_symbols_)
            fprintf(out,"%s ",token_datas[as].name_or_alias().c_str());
        fputc(static_cast<unsigned char>('\n'),out);
        for(const std::basic_string_view<Token_Kind_t> rights : lpl.set_){
            print(out,token_datas,Core<Token_Kind_t>{rights,0});
            fputc(static_cast<unsigned char>('\n'),out);
        }
    }

    template<typename It>
    void propagate(const Lookaheads<Token_Kind_t>& src,It transition) const {
        Token_Kind_t current_cursored=-1, next_cursored=0;
        for(const std::basic_string_view<Token_Kind_t> rights : set_){
            if((next_cursored=rights.front())!=current_cursored){
                while((*transition)->item_set.accessing_symbol()!=next_cursored)
                    ++transition;
                current_cursored=next_cursored;
            }
            const auto item = (*transition)->item_set.find({rights,1});
            assert(item!=(*transition)->item_set.end());
            for(const Token_Kind_t t : src)
                yuki::as_non_const(item->lookaheads_pending).insert(t);
        }
    }
};




template<typename Item,typename Total_Size>
struct LR1_ImmActions{
    typedef typename Item::Token_Kind_t Token_Kind_t;

    yuki::GSet<Token_Kind_t,Item_Set<Item,Total_Size>,Get_Accessing_Symbol> transitions;
    Reduction_Set<Token_Kind_t> empty_reductions;

    void clear() {transitions.clear(); empty_reductions.clear();}

    template<typename Item2,typename Total_Size2,typename... L>
    void merge(const RR_Conflict_Resolver& rrcr,const LR1_ImmActions<Item2,Total_Size2>& src,const L&... l){
        static_assert(std::is_same_v<Token_Kind_t,typename Item2::Token_Kind_t>);
        transitions.merge(
            decltype(transitions)(
                yuki::from_ordered_tag,
                src.transitions.tree().template isomorphic_copy<typename decltype(transitions)::tree_type>(
                    [&l...](const Item_Set<Item2,Total_Size2>& is)->Item_Set<Item,Total_Size> {return {is,l...};}
                )
            ),
            [](Item_Set<Item,Total_Size>& dst1,Item_Set<Item,Total_Size>&& src1)static{
                dst1.merge(std::move(src1));
            }
        );
        empty_reductions.merge(rrcr,src.empty_reductions,l...);
    }
};

template<typename Item,typename Total_Size>
void print(FILE* const out,const Token_Datas& token_datas,const typename Item::Token_Kind_t nterm,const LR1_ImmActions<Item,Total_Size>& ia){
    typedef typename Item::Token_Kind_t Token_Kind_t;
    fprintf(out,"%s\n%s\n%s\n",log_banner,token_datas[nterm].name_or_alias().c_str(),log_banner);
    for(const Item_Set<Item,Total_Size>& item_set : ia.transitions){
        fprintf(out,"%s    %zu %zu (%zu %zu)\n",token_datas[item_set.accessing_symbol()].name_or_alias().c_str(),item_set.core_count(),item_set.total_size(),item_set.reductions_core_count(),item_set.reductions_total_size());
        for(const Item& item : item_set){
            print(out,token_datas,item);
            fprintf(out," %zu\n",item.lookaheads.size());
        }
    }
    fprintf(out,"%%empty %zu\n",ia.empty_reductions.size());
    for(const Reduction<Token_Kind_t> reduction : ia.empty_reductions){
        assert(reduction.left!=(Token_Kind_t)-1);
        fprintf(out,"%s    rule=%zu(%s)  prec_sr=%u  prec_rr=%u\n",token_datas[reduction.term].name_or_alias().c_str(),reduction.rule,token_datas[reduction.left].name_or_alias().c_str(),unsigned(reduction.prec_sr),unsigned(reduction.prec_rr));
    }
    if(ia.empty_reductions.contains(dummy_lookahead)){
        const Reduction<Token_Kind_t> dummy_reduction = ia.empty_reductions.dummy();
        fprintf(out,"#    rule=%zu(%s)  prec_sr=%u  prec_rr=%u\n",dummy_reduction.rule,token_datas[dummy_reduction.left].name_or_alias().c_str(),unsigned(dummy_reduction.prec_sr),unsigned(dummy_reduction.prec_rr));
    }
}

template<typename Item,typename Total_Size>
void print(FILE* const out,const Token_Datas& token_datas,const LR1_ImmActions<Item,Total_Size>* const ias){
    const typename Item::Token_Kind_t nterms_size = token_datas.nterms.size();
    for(typename Item::Token_Kind_t i=0;i<nterms_size;++i)
        print(out,token_datas,i,ias[i]);
}

template<typename Item,typename Total_Size>
void print(FILE* const out,const Token_Datas& token_datas,const LR1_ImmActions<Item,Total_Size>* const ias,const Lookahead_PropList<typename Item::Token_Kind_t>* const lpls){
    const typename Item::Token_Kind_t nterms_size = token_datas.nterms.size();
    for(typename Item::Token_Kind_t i=0;i<nterms_size;++i){
        print(out,token_datas,i,ias[i]);
        fprintf(out,"%%lpl %zu %zu\n",lpls[i].size(),lpls[i].accessing_symbols().size());
        print(out,token_datas,lpls[i]);
    }
}

template<std::unsigned_integral Token_Kind_t_p>
struct LR1_IT_Item{
    typedef Token_Kind_t_p Token_Kind_t;

    Rule_View<Token_Kind_t> rule;
    static constexpr size_t cursor=1;
    Lookaheads<Token_Kind_t> lookaheads;

    template<typename... L>
    constexpr LR1_IT_Item(const Rule_View<Token_Kind_t> rp,const size_t,L&&... lp) noexcept :
        rule(rp),
        lookaheads(std::forward<L>(lp)...)
    {}
};

static_assert(has_lookaheads<LR1_IT_Item<unsigned char>>);

template<std::unsigned_integral Token_Kind_t>
struct Make_LR1_IAs : private Make_FT<Token_Kind_t>{
  private:
    using TC_Context<Token_Kind_t>::rules;
    using TC_Context<Token_Kind_t>::depth;
    using TC_Context<Token_Kind_t>::stack;
    using TC_Context<Token_Kind_t>::stack_size;
    using TC_Context<Token_Kind_t>::nterms_size;
    using Make_FT<Token_Kind_t>::first_table;
    LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* const immediate_actions;
    Lookahead_PropList<Token_Kind_t>* const lookahead_proplists;
    Lookaheads<Token_Kind_t>* const lookaheads_pending;
    Lookaheads<Token_Kind_t> lookaheads_temp;
    FILE* const fp_log;
    const Token_Datas& token_datas;
    size_t rr_conflicts=0;

    static std::basic_string_view<Token_Kind_t> get_tail_string_string(const std::basic_string<Token_Kind_t>& rights,const size_t cursor) {return get_tail_string(std::basic_string_view<Token_Kind_t>(rights),cursor);}

    void traverse(const Token_Kind_t k){
        assert(stack_size<nterms_size);
        stack[stack_size++]=k;
        const Token_Kind_t depth_orig = stack_size;
        depth[k]=depth_orig;

        typename Rule_Set<Token_Kind_t>::const_iterator b = rules.begin(k);
        const typename Rule_Set<Token_Kind_t>::const_iterator e = rules.end(k);
        for(;b!=e;++b){
            const Token_Kind_t rule_first = b->first();
            if(rule_first>=nterms_size)
                break;
            if(depth[rule_first]==0){
                traverse(rule_first);
                lookaheads_temp.clear();
                first_table.populate(lookaheads_temp,nterms_size,get_tail_string_string(b->rights,0),dummy_lookahead);
                immediate_actions[k].merge({fp_log,token_datas,rr_conflicts,0,k,rule_first,"Merging empty-reductions"},immediate_actions[rule_first],lookaheads_temp);
                if(lookaheads_temp.contains(dummy_lookahead))
                    lookahead_proplists[k].merge(lookahead_proplists[rule_first]);
            }else if(rule_first!=k){
                lookaheads_temp.clear();
                first_table.populate(lookaheads_temp,nterms_size,get_tail_string_string(b->rights,0),dummy_lookahead);
                immediate_actions[k].merge({fp_log,token_datas,rr_conflicts,0,k,rule_first,"Merging empty-reductions"},immediate_actions[rule_first],lookaheads_temp);
                if(lookaheads_temp.contains(dummy_lookahead))
                    lookahead_proplists[k].merge(lookahead_proplists[rule_first]);
            }else{
                first_table.populate(lookaheads_pending[k],nterms_size,get_tail_string_string(b->rights,0));
            }
            if(depth[rule_first] < depth[k])
                depth[k] = depth[rule_first];
            yuki::as_non_const(*(immediate_actions[k].transitions.emplace_at(rule_first).iterator)).emplace(*b,1,dummy_lookahead);
            lookahead_proplists[k].insert(b->rights);
        }

        auto sd = [this,k](Item_Set<LR1_IT_Item<Token_Kind_t>,No_Size>& item_set){
            item_set.substitute_dummy(lookaheads_pending[k],dummy_lookahead);
        };
        immediate_actions[k].transitions.tree().recursive_traverse(sd);

        const RR_Conflict_Resolver rrcr_insertion = {fp_log,token_datas,rr_conflicts,0,k};

        for(;b!=e;++b){
            const Token_Kind_t rule_first = b->first();
            if(rule_first==(Token_Kind_t)-1){
                for(const Token_Kind_t term : lookaheads_pending[k])
                    immediate_actions[k].empty_reductions.insert(rrcr_insertion,{term,*b});
                immediate_actions[k].empty_reductions.insert(rrcr_insertion,{(Token_Kind_t)-1,*b});
                break;
            }
            yuki::as_non_const(*(immediate_actions[k].transitions.emplace_at(rule_first).iterator)).emplace(*b,1,lookaheads_pending[k],dummy_lookahead);
            lookahead_proplists[k].insert(b->rights);
        }

        {
        Lookaheads<Token_Kind_t> empty_lh;
        swap(lookaheads_pending[k],empty_lh);
        }

        if(depth[k]==depth_orig){
            while(1){
                const Token_Kind_t pop = stack[--stack_size];
                depth[pop] = -1;
                if(pop==k)
                    return;
                else{
                    immediate_actions[pop]=immediate_actions[k];
                    lookahead_proplists[pop]=lookahead_proplists[k];
                }
            }
        }
    }
  public:
    Make_LR1_IAs(const size_t nsp,const Rule_Set<Token_Kind_t>& rp,FILE* const flp,const Token_Datas& tdp) noexcept :
        Make_FT<Token_Kind_t>(nsp,rp),
        immediate_actions(new LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>[nsp]),
        lookahead_proplists(new Lookahead_PropList<Token_Kind_t>[nsp]),
        lookaheads_pending(new Lookaheads<Token_Kind_t>[nsp]),
        fp_log(flp),
        token_datas(tdp)
    {}

    ~Make_LR1_IAs() noexcept {delete[] lookaheads_pending;}

    struct Ret{
        First_Table<Token_Kind_t> first_table;
        const LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* immediate_actions;
        const Lookahead_PropList<Token_Kind_t>* lookahead_proplists;
        size_t rr_conflicts;
    };

    Ret calc(){
        Make_FT<Token_Kind_t>::calc();
        TC_Context<Token_Kind_t>::reset();
        traverse(0);
        for(Token_Kind_t i=1;i<nterms_size;++i)
            if(depth[i]==0)
                traverse(i);
        return {first_table,immediate_actions,lookahead_proplists,rr_conflicts};
    }
};




template<typename Item,typename Total_Size>
struct Flat_Item_Set{
    typedef typename Item::Token_Kind_t Token_Kind_t;
    typedef Item item_type;
    typedef Total_Size total_size_type;
  private:
    Item* items_=nullptr;
    With_Size<Total_Size> items_size_;
    With_Size<Total_Size> reductions_size_;
  public:
    Flat_Item_Set() noexcept = default;
    Flat_Item_Set(const Flat_Item_Set& other) noexcept :
        items_(yuki::Allocator<Item>::allocate(other.items_size_.core_count())),
        items_size_(other.items_size_),
        reductions_size_(other.reductions_size_)
    {yuki::uninitialized_copy_no_overlap(items_,other.items_,items_size_.core_count());}

    Flat_Item_Set(Flat_Item_Set&& other) noexcept :
        items_(other.items_),
        items_size_(other.items_size_),
        reductions_size_(other.reductions_size_)
    {other.items_=nullptr; other.items_size_.clear(); other.reductions_size_.clear();}

    ~Flat_Item_Set() noexcept {
        yuki::destroy(items_,items_size_.core_count());
        yuki::Allocator<Item>::deallocate(items_,items_size_.core_count());
    }

    template<typename Total_Size2>
    Flat_Item_Set(Item_Set<Item,Total_Size2>&& is) noexcept :
        items_(yuki::Allocator<Item>::allocate(is.core_count())),
        items_size_(is.core_count(),static_cast<Total_Size2>(is)),
        reductions_size_(is.reductions_size_)
    {
        is.Total_Size2::clear();
        is.reductions_size_.clear();
        is.set_.tree().recursive_popfront([i=items_](Item&& item)mutable{::new(i) Item(std::move(item));++i;});
    }

    template<typename It>
    Flat_Item_Set(It it,const It end) noexcept :
        items_(yuki::Allocator<Item>::allocate(end-it)),
        items_size_(end-it),
        reductions_size_()
    {
        Item* current = items_;
        for(;it!=end;++it,++current){
            assert(current==items_ || YUKI_PROTECT(Core<Token_Kind_t>{(current-1)->rule.rights,(current-1)->cursor} < Core<Token_Kind_t>{(*it)->rule.rights,(*it)->cursor}));
            ::new(current) Item(**it);
            items_size_.advance(0,(*it)->lookaheads.size());
            if((*it)->cursor>=(*it)->rule.rights.size())
                reductions_size_.advance(1,(*it)->lookaheads.size());
        }
    }

    Flat_Item_Set& operator=(const Flat_Item_Set&) noexcept = delete;
    Flat_Item_Set& operator=(Flat_Item_Set&&) noexcept = delete;

    bool empty() const {return items_size_.empty();}

    void reset(){
        yuki::destroy(items_,items_size_.core_count());
        yuki::Allocator<Item>::deallocate(items_,items_size_.core_count());
        items_=nullptr;
        items_size_.clear();
        reductions_size_.clear();
    }

    typedef const Item* const_iterator;

    const_iterator begin() const {return items_;}
    const_iterator begin_reductions() const {return items_+(items_size_.core_count()-reductions_size_.core_count());}
    const_iterator end() const {return items_+items_size_.core_count();}

    const Item& operator[](const size_t i) const {assert(i<items_size_.core_count()); return items_[i];}

    const_iterator find(const Core<Token_Kind_t> core) const {
        return yuki::bfind(items_,items_+items_size_.core_count(),core,To_Core{},yuki::Less<Core<Token_Kind_t>>{});
    }

    size_t core_count() const {return items_size_.core_count();}
    size_t total_size() const {return items_size_.size();}
    size_t reductions_core_count() const {return reductions_size_.core_count();}
    size_t reductions_total_size() const {return reductions_size_.size();}

    Token_Kind_t accessing_symbol() const {assert(items_); return get_accessing_symbol(items_->rule.rights,items_->cursor);}
};




struct IsoCore{
    template<typename Item,typename Total_Size,typename Item2,typename Total_Size2>
    static bool operator()(const Flat_Item_Set<Item,Total_Size>& lhs,const Flat_Item_Set<Item2,Total_Size2>& rhs){
        static_assert(std::is_same_v<typename Item::Token_Kind_t,typename Item2::Token_Kind_t>);
        assert(lhs.core_count()==rhs.core_count());
        typename Flat_Item_Set<Item,Total_Size>::const_iterator lb = lhs.begin();
        typename Flat_Item_Set<Item2,Total_Size2>::const_iterator rb = rhs.begin();
        const typename Flat_Item_Set<Item,Total_Size>::const_iterator le = lhs.end();
        for(;lb!=le;++lb,++rb){
            typedef typename Item::Token_Kind_t Token_Kind_t;
            if(lb->rule.rights.data()!=rb->rule.rights.data() || lb->cursor!=rb->cursor)
                return false;
        }
        return true;
    }

    template<typename Vec>
    static constexpr bool operator()(const Vec&) {return true;}
};

/// See "A Practical General Method for Constructing LR(k) Parsers" by David Pager.
struct Weakly_Compatible{
    template<typename Item,typename Total_Size,typename Item2,typename Total_Size2>
    static bool operator()(const Flat_Item_Set<Item,Total_Size>& lhs,const Flat_Item_Set<Item2,Total_Size2>& rhs){
        if(!IsoCore::operator()(lhs,rhs))
            return false;
        const size_t size = lhs.core_count();
        for(size_t i=0;i<size;++i){
            for(size_t j=i+1;j<size;++j){
                if(!(lhs[i].lookaheads.intersects(rhs[j].lookaheads)) && !(rhs[i].lookaheads.intersects(lhs[j].lookaheads)))
                    continue;
                if(lhs[i].lookaheads.intersects(lhs[j].lookaheads))
                    continue;
                if(rhs[i].lookaheads.intersects(rhs[j].lookaheads))
                    continue;
                return false;
            }
        }
        return true;
    }

    template<typename Vec>
    static constexpr bool operator()(const Vec& vec){
        const size_t size = vec.size();
        for(size_t i=0;i<size;++i){
            for(size_t j=i+1;j<size;++j){
                if(!(vec[i]->lookaheads.intersects(vec[j]->lookaheads_pending)) && !(vec[i]->lookaheads_pending.intersects(vec[j]->lookaheads)))
                    continue;
                if(vec[i]->lookaheads.intersects(vec[j]->lookaheads))
                    continue;
                if(vec[i]->lookaheads_pending.intersects(vec[j]->lookaheads_pending))
                    continue;
                return false;
            }
        }
        return true;
    }
};

template<std::unsigned_integral Token_Kind_t_p>
struct LALR1_Item : private Lookaheads_Pair<Token_Kind_t_p>{
    typedef Token_Kind_t_p Token_Kind_t;

    Rule_View<Token_Kind_t> rule;
    size_t cursor;
    using Lookaheads_Pair<Token_Kind_t>::lookaheads;
    using Lookaheads_Pair<Token_Kind_t>::lookaheads_pending;

    Lookaheads<Token_Kind_t>* prop=nullptr;
    bool tail_nullable=false;

    Lookaheads_Pair<Token_Kind_t>& lookaheads_pair() {return *this;}
    const Lookaheads_Pair<Token_Kind_t>& lookaheads_pair() const {return *this;}

    template<typename... L>
    LALR1_Item(const Rule_View<Token_Kind_t> rp,const size_t cp,L&&... lp) noexcept :
        Lookaheads_Pair<Token_Kind_t>(std::forward<L>(lp)...),
        rule(rp),
        cursor(cp)
    {}
};

template<std::unsigned_integral Token_Kind_t>
using LALR1_Item_Set = Item_Set<LALR1_Item<Token_Kind_t>,No_Size>;
template<std::unsigned_integral Token_Kind_t>
using Flat_LALR1_Item_Set = Flat_Item_Set<LALR1_Item<Token_Kind_t>,No_Size>;

template<std::unsigned_integral Token_Kind_t>
struct LALR1_State{
    size_t index;
    Flat_LALR1_Item_Set<Token_Kind_t> item_set;
    Reduction_Set<Token_Kind_t> reductions;
    yuki::Vector<LALR1_State<Token_Kind_t>*> transitions;
    bool multi_parents=false;
    bool merge_pending=false;
    bool self_transition=false;

    bool calculated() const {return !reductions.empty() || !transitions.empty();}

    void reset(){
        reductions.reset();
        transitions.reset();
    }
};

template<typename State>
struct Set;

template<std::unsigned_integral Token_Kind_t>
struct Merge_And_Propagate{
    RR_Conflict_Resolver rrcr;
    const LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* const lr1_ias;
    const Lookahead_PropList<Token_Kind_t>* const lookahead_proplists;
    Lookaheads<Token_Kind_t> nterm_propagate;

    Merge_And_Propagate(const RR_Conflict_Resolver rp,const LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* const lip,const Lookahead_PropList<Token_Kind_t>* const lpp) noexcept :
        rrcr(rp),
        lr1_ias(lip),
        lookahead_proplists(lpp)
    {}
  private:
    Lookaheads<Token_Kind_t> direct_propagate;
    Lookaheads<Token_Kind_t> propagate_ass; /// Accessing-SymbolS

    yuki::Vector<LALR1_Item<Token_Kind_t>*> lalr1_items;
    struct Item_Iterator{
        typename yuki::Vector<LALR1_Item<Token_Kind_t>*>::iterator base;

        LALR1_Item<Token_Kind_t>& operator*() const {return **base;}
        LALR1_Item<Token_Kind_t>* operator->() const {return *base;}
        Item_Iterator& operator++() {++base;return *this;}

        bool operator==(const Item_Iterator& other) const noexcept = default;

        Lookaheads<Token_Kind_t>& pending() const {return (*base)->lookaheads_pending;}
    };

    struct Nterm_Propagate{
        Token_Kind_t nterm;
        Lookaheads<Token_Kind_t> lookaheads;
    };
    struct Direct_Propagate{
        Lookaheads<Token_Kind_t>* prop;
        Lookaheads<Token_Kind_t> lookaheads;
    };
    yuki::Vector<Nterm_Propagate> nterm_propagate_recipe;
    yuki::Vector<Direct_Propagate> direct_propagate_recipe;

    size_t secondary_incompatibilities = 0;
  public:
    void print_secondary_incompatibilities(FILE* const out) const {
        if(secondary_incompatibilities)
            fprintf(out,"Warning: %zu incompatibilities during propagation. See log for details.\n",secondary_incompatibilities);
    }

    template<typename Eq,typename It>
    void doo(LALR1_State<Token_Kind_t>& state,It it,const It reduc,const It end);
};

template<typename State>
struct Set{
  private:
    typedef decltype(State{}.item_set) item_set_type;
    typedef typename item_set_type::item_type item_type;
    struct Get_Items{
        static auto& operator()(State& state) {return state.item_set;}
        static const auto& operator()(const State& state) {return state.item_set;}
    };
    struct Item_Set_Less{
        static std::strong_ordering compare3(const item_set_type& lhs,const item_set_type& rhs){
            if(const std::strong_ordering cmp=lhs.core_count()<=>rhs.core_count(); cmp!=0) return cmp;
            if(lhs.core_count()!=0)
                if(const std::strong_ordering cmp=lhs.accessing_symbol()<=>rhs.accessing_symbol(); cmp!=0) return cmp;
            else
                return std::strong_ordering::equal;
            if(const std::strong_ordering cmp=lhs.total_size()<=>rhs.total_size(); cmp!=0) return cmp;
            if(const std::strong_ordering cmp=lhs.reductions_core_count()<=>rhs.reductions_core_count(); cmp!=0) return cmp;
            if(const std::strong_ordering cmp=lhs.reductions_total_size()<=>rhs.reductions_total_size(); cmp!=0) return cmp;
            return std::strong_ordering::equal;
        }
        static bool operator()(const item_set_type& lhs,const item_set_type& rhs) {return compare3(lhs,rhs)<0;}
    };
    typedef yuki::RB_Tree<item_set_type,State,Get_Items,Item_Set_Less> tree_type;
    tree_type tree_;

    struct Double_Item_Iterator{
        typename item_set_type::const_iterator l;
        typename item_set_type::const_iterator r;

        auto& operator*() const {return yuki::as_non_const(*l);}
        auto operator->() const {using yuki::const_kast; return const_kast(l);}
        Double_Item_Iterator& operator++() {++l;++r;return *this;}

        bool operator==(const Double_Item_Iterator other) const noexcept {return l==other.l;}

        Lookaheads<typename item_set_type::Token_Kind_t>& pending() const {return yuki::as_non_const(r->lookaheads);}
    };
  public:
    size_t size() const {return tree_.size();}

    typedef typename tree_type::const_iterator const_iterator;

    template<typename Eq>
    yuki::IB_Pair<const_iterator> insert(item_set_type&& item_set){
        const_iterator feg = tree_.first_equiv_greater(item_set);
        const const_iterator fg = tree_.first_greater(item_set);

        for(;feg!=fg;++feg){
            if(Eq::operator()(feg->item_set,item_set))
                return {feg,false};
        }
        return {tree_.emplace(size()+1,std::move(item_set)),true};
    }

    template<typename Eq,bool self=false>
    yuki::IB_Pair<const_iterator> insert_and_merge(item_set_type&& item_set,Merge_And_Propagate<typename item_set_type::Token_Kind_t>& merge_and_propagate,State*& transition,const const_iterator current_state=const_iterator()){
        const yuki::IB_Pair<const_iterator> ibp = insert<Eq>(std::move(item_set));
        transition=&(yuki::as_non_const(*ibp.iterator));
        if(!ibp.has_inserted){
            if constexpr(self)
                if(ibp.iterator==current_state)
                    yuki::as_non_const(current_state->self_transition)=true;
            yuki::as_non_const(ibp.iterator->multi_parents)=true;
            if(ibp.iterator->calculated()){
                merge_and_propagate.template doo<Eq>(yuki::as_non_const(*ibp.iterator),Double_Item_Iterator{ibp.iterator->item_set.begin(),item_set.begin()},Double_Item_Iterator{ibp.iterator->item_set.begin_reductions(),item_set.begin_reductions()},Double_Item_Iterator{ibp.iterator->item_set.end(),item_set.end()});
            }else{
                typename item_set_type::const_iterator lb = ibp.iterator->item_set.begin();
                typename item_set_type::const_iterator rb = item_set.begin();
                const typename item_set_type::const_iterator re = item_set.end();
                for(;rb!=re;++lb,++rb)
                    yuki::as_non_const(lb->lookaheads).merge(std::move(yuki::as_non_const(rb->lookaheads)));
            }
            item_set.reset();
        }
        return ibp;
    }

    const_iterator force_insert(item_set_type&& item_set){
        return tree_.emplace(size()+1,std::move(item_set));
    }

    void insert_accept() {tree_.emplace(0);}
};




template<std::unsigned_integral Token_Kind_t>
template<typename Eq,typename It>
void Merge_And_Propagate<Token_Kind_t>::doo(LALR1_State<Token_Kind_t>& state,It item,const It reduc,const It items_end){
    assert(nterm_propagate.empty());
    rrcr.state=state.index;
    auto transition = state.transitions.begin();
    const auto transitions_end = state.transitions.end();
    {
    const auto transitions_begin = transition;
    Token_Kind_t current_cursored=-1, next_cursored=0;
    bool current_cursored_direct_prop = false;
    // Merge non-terminal-cursored items.
    while(1){
        if(item==reduc){
            if(current_cursored!=(Token_Kind_t)-1){
                if(!nterm_propagate.empty()){
                    propagate_ass.merge(lookahead_proplists[current_cursored].accessing_symbols());
                    if(!state.self_transition){
                        lookahead_proplists[current_cursored].propagate(nterm_propagate,transitions_begin);
                        if(lr1_ias[current_cursored].empty_reductions.contains(dummy_lookahead)){
                            rrcr.src=current_cursored;
                            rrcr.occasion_str="Expanding dummy-empty-reduction (while merging state)";
                            state.reductions.expand_dummy(rrcr,lr1_ias[current_cursored].empty_reductions.dummy(),nterm_propagate);
                        }
                    }else
                        nterm_propagate_recipe.emplace_back(current_cursored,nterm_propagate);
                    nterm_propagate.clear();
                }
                if(current_cursored_direct_prop){
                    current_cursored_direct_prop=false;
                    propagate_ass.insert(current_cursored);
                }
            }
            goto merge_reductions;
        }
        if((next_cursored=get_cursored(item->rule.rights,item->cursor))!=current_cursored){
            if(current_cursored!=(Token_Kind_t)-1){
                if(!nterm_propagate.empty()){
                    propagate_ass.merge(lookahead_proplists[current_cursored].accessing_symbols());
                    if(!state.self_transition){
                        lookahead_proplists[current_cursored].propagate(nterm_propagate,transitions_begin);
                        if(lr1_ias[current_cursored].empty_reductions.contains(dummy_lookahead)){
                            rrcr.src=current_cursored;
                            rrcr.occasion_str="Expanding dummy-empty-reduction (while merging state)";
                            state.reductions.expand_dummy(rrcr,lr1_ias[current_cursored].empty_reductions.dummy(),nterm_propagate);
                        }
                    }else
                        nterm_propagate_recipe.emplace_back(current_cursored,nterm_propagate);
                    nterm_propagate.clear();
                }
                if(current_cursored_direct_prop){
                    current_cursored_direct_prop=false;
                    propagate_ass.insert(current_cursored);
                }
            }
            while((*transition)->item_set.accessing_symbol()!=next_cursored)
                ++transition;
            current_cursored=next_cursored;
            if(!rrcr.token_datas.is_nterm(current_cursored))
                break;
        }
        if(item->tail_nullable){
            for(const Token_Kind_t t : item.pending()){
                if(item->lookaheads.insert(t)){
                    direct_propagate.force_insert(t);
                    nterm_propagate.insert(t);
                }
            }
        }else{
            for(const Token_Kind_t t : item.pending()){
                if(item->lookaheads.insert(t)){
                    direct_propagate.force_insert(t);
                }
            }
        }
        item.pending().clear();
        if(!direct_propagate.empty()){
            current_cursored_direct_prop=true;
            if(!item->prop){
                item->prop = &(yuki::as_non_const((*transition)->item_set.find({item->rule.rights,item->cursor+1})->lookaheads_pending));
            }
            if(!state.self_transition){
                assert(item->prop->empty());
                item->prop->merge(direct_propagate);
            }else
                direct_propagate_recipe.emplace_back(item->prop,direct_propagate);
            direct_propagate.clear();
            if((*transition)->index==0)
                item->prop=nullptr;
        }
        ++item;
    } // while(1)
    // Merge terminal-cursored items.
    while(1){
        if(item==reduc){
            if(current_cursored!=(Token_Kind_t)-1){
                if(current_cursored_direct_prop){
                    current_cursored_direct_prop=false;
                    propagate_ass.insert(current_cursored);
                }
            }else
                assert(transitions_begin==transitions_end);
            goto merge_reductions;
        }
        if((next_cursored=get_cursored(item->rule.rights,item->cursor))!=current_cursored){
            if(current_cursored!=(Token_Kind_t)-1){
                if(current_cursored_direct_prop){
                    current_cursored_direct_prop=false;
                    propagate_ass.insert(current_cursored);
                }
            }
            while((*transition)->item_set.accessing_symbol()!=next_cursored)
                ++transition;
            current_cursored=next_cursored;
        }
        for(const Token_Kind_t t : item.pending()){
            if(item->lookaheads.insert(t)){
                direct_propagate.force_insert(t);
            }
        }
        item.pending().clear();
        if(!direct_propagate.empty()){
            current_cursored_direct_prop=true;
            if(!item->prop){
                item->prop = &(yuki::as_non_const((*transition)->item_set.find({item->rule.rights,item->cursor+1})->lookaheads_pending));
            }
            if(!state.self_transition){
                assert(item->prop->empty());
                item->prop->merge(direct_propagate);
            }else
                direct_propagate_recipe.emplace_back(item->prop,direct_propagate);
            direct_propagate.clear();
            if((*transition)->index==0)
                item->prop=nullptr;
        }
        ++item;
    } // while(1)
    //
    merge_reductions:
    assert(item==reduc);
    rrcr.src=-1;
    rrcr.occasion_str="Merging state";
    for(;item!=items_end;++item){
        Reduction<Token_Kind_t> reduc(0,item->rule);
        for(const Token_Kind_t t : item.pending()){
            if(item->lookaheads.insert(t)){
                reduc.term=t;
                state.reductions.insert(rrcr,reduc);
            }
        }
        item.pending().clear();
    }
    if constexpr(std::is_same_v<It,Item_Iterator>)
        lalr1_items.clear();
    // Perform out-of-place propagation.
    if(state.self_transition){
        while(!nterm_propagate_recipe.empty()){
            const Nterm_Propagate np = nterm_propagate_recipe.pop_back_v();
            lookahead_proplists[np.nterm].propagate(np.lookaheads,transitions_begin);
            if(lr1_ias[np.nterm].empty_reductions.contains(dummy_lookahead)){
                rrcr.src=np.nterm;
                rrcr.occasion_str="Expanding dummy-empty-reduction (while merging state)";
                state.reductions.expand_dummy(rrcr,lr1_ias[np.nterm].empty_reductions.dummy(),np.lookaheads);
            }
        }
        while(!direct_propagate_recipe.empty()){
            const Direct_Propagate dp = direct_propagate_recipe.pop_back_v();
            dp.prop->merge(std::move(dp.lookaheads));
        }
    }
    }
    //
    if(!propagate_ass.empty()){
        { // Mark all transitions that have been propagated, so that `propagate_ass` can be reused in the recursive calls.
        transition = state.transitions.begin();
        typename Lookaheads<Token_Kind_t>::const_iterator pab = propagate_ass.begin();
        const typename Lookaheads<Token_Kind_t>::const_iterator pae = propagate_ass.end();
        for(;pab!=pae;++pab){
            assert((*transition)->item_set.accessing_symbol() <= *pab);
            while((*transition)->item_set.accessing_symbol() < *pab)
                ++transition;
            if(!(*transition)->calculated() && (std::is_same_v<Eq,IsoCore> || !(*transition)->multi_parents)){
                typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator b = (*transition)->item_set.begin();
                const typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator e = (*transition)->item_set.end();
                for(;b!=e;++b){
                    yuki::as_non_const(b->lookaheads).merge(std::move(yuki::as_non_const(b->lookaheads_pending)));
                    yuki::as_non_const(b->lookaheads_pending).clear();
                }
            }else
                (*transition)->merge_pending=true;
        }
        propagate_ass.clear();
        }
        // Merge and propagate the marked transitions.
        transition = state.transitions.begin();
        for(;transition!=transitions_end;++transition){
            if((*transition)->merge_pending){
                assert(lalr1_items.empty());
                size_t lalr1_items_reduc=0;
                bool lalr1_items_has_reduc=false;
                (*transition)->merge_pending=false;
                for(const auto& transition_item : (*transition)->item_set){
                    if(!transition_item.lookaheads_pending.empty()){
                        if(!lalr1_items_has_reduc && (transition_item.cursor>=transition_item.rule.rights.size())){
                            lalr1_items_has_reduc=true;
                            lalr1_items_reduc=lalr1_items.size();
                        }
                        lalr1_items.emplace_back(&yuki::as_non_const(transition_item));
                    }
                }
                if(!lalr1_items_has_reduc)
                    lalr1_items_reduc=lalr1_items.size();
                if(Eq::operator()(lalr1_items)){
                    if(!(*transition)->calculated()){
                        typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator b = (*transition)->item_set.begin();
                        const typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator e = (*transition)->item_set.end();
                        for(;b!=e;++b){
                            yuki::as_non_const(b->lookaheads).merge(std::move(yuki::as_non_const(b->lookaheads_pending)));
                            yuki::as_non_const(b->lookaheads_pending).clear();
                        }
                    }else
                        doo<Eq>(**transition,Item_Iterator{lalr1_items.begin()},Item_Iterator{lalr1_items.begin()+lalr1_items_reduc},Item_Iterator{lalr1_items.end()});
                }else{
                    if((*transition)->multi_parents){
                        ++secondary_incompatibilities;
                        if(rrcr.out)
                            fprintf(rrcr.out,"Incompatibility when propagating to state %zu.\n",(*transition)->index);
                    }
                    doo<Eq>(**transition,Item_Iterator{lalr1_items.begin()},Item_Iterator{lalr1_items.begin()+lalr1_items_reduc},Item_Iterator{lalr1_items.end()});
                }
            } // if((*transition)->merge_pending)
        } // for(;transition!=transitions_end;++transition)
    } // if(!propagate_ass.empty())
}

template<std::unsigned_integral Token_Kind_t>
struct Reducible{
    const Token_Kind_t left;
    bool reducible=false;
};

#define IND YUKI_PG_IND
#define IND2 IND IND
#define IND3 IND IND IND
#define IND4 IND IND IND IND
#define IND5 IND IND IND IND IND
#define IND6 IND IND IND IND IND IND

/// @return RS-conflicts
template<typename Item,typename Total_Size,typename State>
size_t write_lr1_initial_actions(
    const Token_Datas& token_datas,
    FILE* const fp_file,FILE* const fp_log,
    LR1_ImmActions<Item,Total_Size>& current_actions,
    Set<State>& states,
    yuki::RingQueue<typename Set<State>::const_iterator>& worklist,
    Reducible<typename Item::Token_Kind_t>* const reducible)
{
    typedef typename Item::Token_Kind_t Token_Kind_t;
    typedef decltype(State{}.item_set) item_set_type;
    size_t rs_conflicts=0;
    fputs("case 0: S0: switch(t_.kind()){\n",fp_file);
    auto b = current_actions.transitions.begin();
    const auto e = current_actions.transitions.end();
    size_t ghost_goal_state=0;
    for(;b!=e && b->accessing_symbol()<token_datas.goal;++b){
        const typename Set<State>::const_iterator inserted = states.force_insert(item_set_type(std::move(yuki::as_non_const(*b))));
        worklist.emplace_back(inserted);
        fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[inserted->item_set.accessing_symbol()].name_or_alias().c_str(),states.size(),states.size());
        if(fp_log) fprintf(fp_log,"GOTO(0,%s)=%zu\n",token_datas[inserted->item_set.accessing_symbol()].name_or_alias().c_str(),states.size());
    }
    if(b->accessing_symbol()>token_datas.goal){
        states.insert_accept();
        ghost_goal_state=states.size();
    }else{
        assert(b->accessing_symbol()==token_datas.goal);
        yuki::as_non_const(*b).emplace(Rule_View<Token_Kind_t>(make_goal_rule_view_tag),1,token_datas.eof());
        const typename Set<State>::const_iterator inserted = states.force_insert(item_set_type(std::move(yuki::as_non_const(*b))));
        worklist.emplace_back(inserted);
        ++b;
    }
    fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[token_datas.goal].name_or_alias().c_str(),states.size(),states.size());
    if(fp_log) fprintf(fp_log,"GOTO(0,%s)=%zu\n",token_datas[token_datas.goal].name_or_alias().c_str(),states.size());
    for(;b!=e && token_datas.is_nterm(b->accessing_symbol());++b){
        const typename Set<State>::const_iterator inserted = states.force_insert(item_set_type(std::move(yuki::as_non_const(*b))));
        worklist.emplace_back(inserted);
        fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[inserted->item_set.accessing_symbol()].name_or_alias().c_str(),states.size(),states.size());
        if(fp_log) fprintf(fp_log,"GOTO(0,%s)=%zu\n",token_datas[inserted->item_set.accessing_symbol()].name_or_alias().c_str(),states.size());
    }
    for(;b!=e;++b){
        const Token_Kind_t current_token = b->accessing_symbol();
        const Token_Data& current_td = token_datas[current_token];
        if(!current_actions.empty_reductions.resolve_rs_conflict({fp_log,token_datas,rs_conflicts,0},current_token,current_td))
            continue;
        const typename Set<State>::const_iterator inserted = states.force_insert(item_set_type(std::move(yuki::as_non_const(*b))));
        worklist.emplace_back(inserted);
        fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",current_td.name_or_alias().c_str(),states.size(),states.size());
        if(fp_log) fprintf(fp_log,"GOTO(0,%s)=%zu\n",current_td.name_or_alias().c_str(),states.size());
    }
    for(const Reduction<Token_Kind_t> empty_reduction : current_actions.empty_reductions){
        if(empty_reduction.valid()){
            fprintf(fp_file,IND "case %s: p_.SET_STATE(%zu); return {%s,%zu};\n",token_datas[empty_reduction.term].name_or_alias().c_str(),empty_reduction.rule,token_datas[empty_reduction.term].name_or_alias().c_str(),empty_reduction.rule);
            reducible[empty_reduction.rule].reducible=true;
        }
    }
    current_actions.clear();
    fputs(IND "default: return {(size_t)-1,(size_t)-1};\n} // case 0\n",fp_file);
    if(ghost_goal_state!=0){
        fprintf(fp_file,
            "case %zu: S%zu: if(t_.kind()==EOF_){p_.SET_STATE(0); return {EOF_,0};} else return {(size_t)-1,(size_t)-1};\n",
            ghost_goal_state,ghost_goal_state
        );
    }
    return rs_conflicts;
}

template<typename Eq,std::unsigned_integral Token_Kind_t>
size_t write_lalr1(
    FILE* const fp_file,FILE* const fp_err,FILE* const fp_log,
    const Token_Datas& token_datas,
    const typename Make_LR1_IAs<Token_Kind_t>::Ret lr1_ias_ret,
    Reducible<Token_Kind_t>* const reducible, const size_t total_rules,
    yuki::Vector<std::basic_string<Token_Kind_t>>& admissible_terms)
{
    assert(fp_file); assert(fp_err); // Log-out is optional.

    const First_Table<Token_Kind_t> first_table = lr1_ias_ret.first_table;
    const LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* const lr1_ias = lr1_ias_ret.immediate_actions;
    const Lookahead_PropList<Token_Kind_t>* const lookahead_proplists = lr1_ias_ret.lookahead_proplists;
    size_t rr_conflicts = lr1_ias_ret.rr_conflicts;

    LR1_ImmActions<LALR1_Item<Token_Kind_t>,No_Size> current_actions;

    Set<LALR1_State<Token_Kind_t>> states;

    yuki::RingQueue<typename Set<LALR1_State<Token_Kind_t>>::const_iterator> worklist;

    current_actions.merge({nullptr,token_datas,rr_conflicts,0,0},lr1_ias[token_datas.goal],token_datas.eof()); // There shouldn't be any rr-conflicts during this merge.
    size_t rs_conflicts=write_lr1_initial_actions(token_datas,fp_file,fp_log,current_actions,states,worklist,reducible);
    yuki::Vector<typename Set<LALR1_State<Token_Kind_t>>::const_iterator> states_linear;
    for(const typename Set<LALR1_State<Token_Kind_t>>::const_iterator it : worklist)
        states_linear.emplace_back(it);

    yuki::Vector<LALR1_State<Token_Kind_t>> current_actions_linear;

    Merge_And_Propagate<Token_Kind_t> merge_and_propagate{{fp_log,token_datas,rr_conflicts,0,0,0},lr1_ias,lookahead_proplists};

    while(!worklist.empty()){
        const typename Set<LALR1_State<Token_Kind_t>>::const_iterator current_state = worklist.pop_front_v();
        typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator current_item = current_state->item_set.begin();
        const typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator reduction_item = current_state->item_set.begin_reductions();
        const typename Flat_LALR1_Item_Set<Token_Kind_t>::const_iterator items_end = current_state->item_set.end();
        Token_Kind_t current_cursored=-1, next_cursored=0;
        typename decltype(current_actions.transitions)::const_iterator current_transitioned;
        merge_and_propagate.rrcr.state=current_state->index;
        merge_and_propagate.rrcr.occasion_str="";
        // Process non-terminal-cursored items.
        while(1){
            if(current_item==reduction_item){
                if(current_cursored!=(Token_Kind_t)-1){
                    merge_and_propagate.rrcr.src=current_cursored;
                    current_actions.merge(merge_and_propagate.rrcr,lr1_ias[current_cursored],merge_and_propagate.nterm_propagate);
                    merge_and_propagate.nterm_propagate.clear();
                }
                break;
            }
            if((next_cursored=get_cursored(current_item->rule.rights,current_item->cursor))!=current_cursored){
                if(current_cursored!=(Token_Kind_t)-1){
                    merge_and_propagate.rrcr.src=current_cursored;
                    current_actions.merge(merge_and_propagate.rrcr,lr1_ias[current_cursored],merge_and_propagate.nterm_propagate);
                    merge_and_propagate.nterm_propagate.clear();
                }
                current_cursored=next_cursored;
                current_transitioned = current_actions.transitions.emplace_at(current_cursored).iterator;
                if(!token_datas.is_nterm(current_cursored))
                    break;
            }
            yuki::as_non_const(current_item->tail_nullable) = first_table.populate(merge_and_propagate.nterm_propagate,token_datas.nterms.size(),get_tail_string(current_item->rule.rights,current_item->cursor),current_item->lookaheads);
            yuki::as_non_const(*current_transitioned).emplace(current_item->rule,current_item->cursor+1,current_item->lookaheads);
            ++current_item;
        }
        // Process terminal-cursored items.
        for(;current_item!=reduction_item;++current_item){
            if((next_cursored=get_cursored(current_item->rule.rights,current_item->cursor))!=current_cursored){
                current_cursored=next_cursored;
                current_transitioned = current_actions.transitions.emplace_at(current_cursored).iterator;
            }
            yuki::as_non_const(*current_transitioned).emplace(current_item->rule,current_item->cursor+1,current_item->lookaheads);
        }
        // Process reduction items.
        yuki::as_non_const(current_state->reductions) = std::move(current_actions.empty_reductions);
        merge_and_propagate.rrcr.src=-1;
        for(;current_item!=items_end;++current_item){
            Reduction<Token_Kind_t> reduc(0,current_item->rule);
            for(const Token_Kind_t t : current_item->lookaheads){
                reduc.term=t;
                yuki::as_non_const(current_state->reductions).insert(merge_and_propagate.rrcr,reduc);
            }
        }
        // Transform the transitioned item-sets to tentative states.
        current_actions.transitions.tree().recursive_popfront([&current_actions_linear](LALR1_Item_Set<Token_Kind_t>&& item_set){current_actions_linear.emplace_back(0,std::move(item_set));});
        Token_Kind_t self_transition_pending=-1;
        const Token_Kind_t current_state_as = current_state->item_set.accessing_symbol();
        for(LALR1_State<Token_Kind_t>& state : current_actions_linear){
            if(state.item_set.accessing_symbol()==current_state_as)
                self_transition_pending = current_state->transitions.size();
            yuki::as_non_const(current_state->transitions).emplace_back(&state);
        }
        // Process the tentative states.
        if(self_transition_pending != (Token_Kind_t)-1){
            LALR1_State<Token_Kind_t>*& transition = yuki::as_non_const(current_state->transitions)[self_transition_pending];
            const yuki::IB_Pair<typename Set<LALR1_State<Token_Kind_t>>::const_iterator> ibp = states.template insert_and_merge<Eq,true>(std::move(transition->item_set),merge_and_propagate,transition,current_state);
            if(ibp.has_inserted){
                states_linear.emplace_back(ibp.iterator);
                worklist.emplace_back(ibp.iterator);
            }
            if(fp_log) fprintf(fp_log,"GOTO(%zu,%s)=%zu\n",current_state->index,token_datas[current_state->item_set.accessing_symbol()].name_or_alias().c_str(),ibp.iterator->index);
        }
        for(LALR1_State<Token_Kind_t>*& transition : yuki::as_non_const(current_state->transitions)){
            const Token_Kind_t current_transition_as = transition->item_set.accessing_symbol();
            if(current_transition_as!=current_state_as){
                const yuki::IB_Pair<typename Set<LALR1_State<Token_Kind_t>>::const_iterator> ibp = states.template insert_and_merge<Eq>(std::move(transition->item_set),merge_and_propagate,transition);
                if(ibp.has_inserted){
                    states_linear.emplace_back(ibp.iterator);
                    worklist.emplace_back(ibp.iterator);
                }
                if(fp_log) fprintf(fp_log,"GOTO(%zu,%s)=%zu\n",current_state->index,token_datas[current_transition_as].name_or_alias().c_str(),ibp.iterator->index);
            }
        }
        current_actions_linear.clear();
    } // while(!worklist.empty())

    first_table.free();
    delete[] lr1_ias;
    delete[] lookahead_proplists;

    admissible_terms.reserve(states_linear.size());
    for(const typename Set<LALR1_State<Token_Kind_t>>::const_iterator state : states_linear){
        fprintf(fp_file,"case %zu: S%zu: switch(t_.kind()){\n",state->index,state->index);
        typename yuki::Vector<LALR1_State<Token_Kind_t>*>::const_iterator transition = state->transitions.begin();
        const typename yuki::Vector<LALR1_State<Token_Kind_t>*>::const_iterator transitions_end = state->transitions.end();
        Reduction_Set<Token_Kind_t>& reduction_set = yuki::as_non_const(state->reductions);
        std::basic_string<Token_Kind_t> current_admissible_terms;
        while(1){
            if(transition==transitions_end)
                goto print_reductions;
            if(!token_datas.is_nterm((*transition)->item_set.accessing_symbol()))
                break;
            fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[(*transition)->item_set.accessing_symbol()].name_or_alias().c_str(),(*transition)->index,(*transition)->index);
            ++transition;
        }
        for(;transition!=transitions_end;++transition){
            if(!reduction_set.resolve_rs_conflict({fp_log,token_datas,rs_conflicts,state->index},(*transition)->item_set.accessing_symbol(),token_datas[(*transition)->item_set.accessing_symbol()]))
                continue;
            fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[(*transition)->item_set.accessing_symbol()].name_or_alias().c_str(),(*transition)->index,(*transition)->index);
            current_admissible_terms.push_back((*transition)->item_set.accessing_symbol());
        }
        print_reductions:
        for(const Reduction<Token_Kind_t> reduction : reduction_set){
            if(reduction.valid()){
                fprintf(fp_file,IND "case %s: p_.SET_STATE(%zu); return {%s,%zu};\n",token_datas[reduction.term].name_or_alias().c_str(),reduction.rule,token_datas[reduction.term].name_or_alias().c_str(),reduction.rule);
                reducible[reduction.rule].reducible=true;
                current_admissible_terms.push_back(reduction.term);
            }
        }
        fprintf(fp_file,IND "default: return {(size_t)-1,(size_t)-1};\n} // case %zu\n",state->index);
        yuki::as_non_const(*state).reset();
        admissible_terms.emplace_back(std::move(current_admissible_terms));
    }

    for(size_t i=1;i<total_rules;++i)
        if(!reducible[i].reducible)
            fprintf(fp_err,"Warning: Non-reducible production %zu (%s) !\n",i,token_datas[reducible[i].left].name_or_alias().c_str());

    merge_and_propagate.print_secondary_incompatibilities(fp_err);

    if(rs_conflicts || rr_conflicts)
        fprintf(fp_err,"%zu RS-conflict(s), %zu RR-conflict(s) in total. See log for details.\n",rs_conflicts,rr_conflicts);

    return states.size()+1;
} // write_lalr1




template<std::unsigned_integral Token_Kind_t_p>
struct CLR1_Item{
    typedef Token_Kind_t_p Token_Kind_t;

    Rule_View<Token_Kind_t> rule;
    size_t cursor;
    Lookaheads<Token_Kind_t> lookaheads;

    template<typename... L>
    CLR1_Item(const Rule_View<Token_Kind_t> rp,const size_t cp,L&&... lp) noexcept :
        rule(rp),
        cursor(cp),
        lookaheads(std::forward<L>(lp)...)
    {}
};

template<std::unsigned_integral Token_Kind_t>
using CLR1_Item_Set = Item_Set<CLR1_Item<Token_Kind_t>,One_Size>;

struct Equal{
    template<std::unsigned_integral Token_Kind_t>
    static bool operator()(const CLR1_Item_Set<Token_Kind_t>& lhs,const CLR1_Item_Set<Token_Kind_t>& rhs){
        assert(lhs.core_count()==rhs.core_count());
        typename CLR1_Item_Set<Token_Kind_t>::const_iterator lb = lhs.begin();
        typename CLR1_Item_Set<Token_Kind_t>::const_iterator rb = rhs.begin();
        const typename CLR1_Item_Set<Token_Kind_t>::const_iterator le = lhs.end();
        for(;lb!=le;++lb,++rb){
            if(lb->rule.rights.data()!=rb->rule.rights.data() || lb->cursor!=rb->cursor || lb->lookaheads!=rb->lookaheads)
                return false;
        }
        return true;
    }
};

template<std::unsigned_integral Token_Kind_t>
size_t write_clr1(
    FILE* const fp_file,FILE* const fp_err,FILE* const fp_log,
    const Token_Datas& token_datas,
    const typename Make_LR1_IAs<Token_Kind_t>::Ret lr1_ias_ret,
    Reducible<Token_Kind_t>* const reducible, const size_t total_rules,
    yuki::Vector<std::basic_string<Token_Kind_t>>& admissible_terms)
{
    assert(fp_file); assert(fp_err); // Log-out is optional.

    delete[] lr1_ias_ret.lookahead_proplists;

    const First_Table<Token_Kind_t> first_table = lr1_ias_ret.first_table;
    const LR1_ImmActions<LR1_IT_Item<Token_Kind_t>,No_Size>* const lr1_ias = lr1_ias_ret.immediate_actions;
    size_t rr_conflicts = lr1_ias_ret.rr_conflicts;

    LR1_ImmActions<CLR1_Item<Token_Kind_t>,No_Size> current_actions;

    struct CLR1_State{
        size_t index;
        CLR1_Item_Set<Token_Kind_t> item_set;
    };

    Set<CLR1_State> states;

    yuki::RingQueue<typename Set<CLR1_State>::const_iterator> worklist;

    current_actions.merge({nullptr,token_datas,rr_conflicts,0,0},lr1_ias[token_datas.goal],token_datas.eof()); // There shouldn't be any rr-conflicts during this merge.
    size_t rs_conflicts=write_lr1_initial_actions(token_datas,fp_file,fp_log,current_actions,states,worklist,reducible);

    Lookaheads<Token_Kind_t> nterm_lookaheads;

    RR_Conflict_Resolver rrcr = {fp_log,token_datas,rr_conflicts,0,0};

    while(!worklist.empty()){
        const typename Set<CLR1_State>::const_iterator current_state = worklist.pop_front_v();
        typename CLR1_Item_Set<Token_Kind_t>::const_iterator current_item = current_state->item_set.begin();
        const typename CLR1_Item_Set<Token_Kind_t>::const_iterator items_end = current_state->item_set.end();
        Token_Kind_t current_cursored=-1, next_cursored=0;
        typename decltype(current_actions.transitions)::const_iterator current_transitioned;
        rrcr.state=current_state->index;
        rrcr.occasion_str="";
        // Process non-terminal-cursored items.
        while(1){
            if(current_item==items_end || current_item->cursor>=current_item->rule.rights.size()){
                if(current_cursored!=(Token_Kind_t)-1){
                    rrcr.src=current_cursored;
                    current_actions.merge(rrcr,lr1_ias[current_cursored],nterm_lookaheads);
                    nterm_lookaheads.clear();
                }
                break;
            }
            if((next_cursored=get_cursored(current_item->rule.rights,current_item->cursor))!=current_cursored){
                if(current_cursored!=(Token_Kind_t)-1){
                    rrcr.src=current_cursored;
                    current_actions.merge(rrcr,lr1_ias[current_cursored],nterm_lookaheads);
                    nterm_lookaheads.clear();
                }
                current_cursored=next_cursored;
                current_transitioned = current_actions.transitions.emplace_at(current_cursored).iterator;
                if(!token_datas.is_nterm(current_cursored))
                    break;
            }
            first_table.populate(nterm_lookaheads,token_datas.nterms.size(),get_tail_string(current_item->rule.rights,current_item->cursor),current_item->lookaheads);
            yuki::as_non_const(*current_transitioned).emplace(current_item->rule,current_item->cursor+1,current_item->lookaheads);
            ++current_item;
        }
        // Process terminal-cursored items.
        for(;current_item!=items_end && current_item->cursor<current_item->rule.rights.size(); ++current_item){
            if((next_cursored=get_cursored(current_item->rule.rights,current_item->cursor))!=current_cursored){
                current_cursored=next_cursored;
                current_transitioned = current_actions.transitions.emplace_at(current_cursored).iterator;
            }
            yuki::as_non_const(*current_transitioned).emplace(current_item->rule,current_item->cursor+1,current_item->lookaheads);
        }
        // Process reduction items.
        rrcr.src=-1;
        for(;current_item!=items_end;++current_item){
            Reduction<Token_Kind_t> reduc(0,current_item->rule);
            for(const Token_Kind_t t : current_item->lookaheads){
                reduc.term=t;
                current_actions.empty_reductions.insert(rrcr,reduc);
            }
        }
        // Process the transitioned item-sets and write out.
        fprintf(fp_file,"case %zu: S%zu: switch(t_.kind()){\n",current_state->index,current_state->index);
        current_transitioned = current_actions.transitions.begin();
        const auto transitions_end = current_actions.transitions.end();
        std::basic_string<Token_Kind_t> current_admissible_terms;
        while(1){
            if(current_transitioned==transitions_end)
                goto print_reductions;
            if(!token_datas.is_nterm(current_transitioned->accessing_symbol()))
                break;
            const yuki::IB_Pair<typename Set<CLR1_State>::const_iterator> ibp = states.template insert<Equal>(CLR1_Item_Set<Token_Kind_t>(std::move(yuki::as_non_const(*current_transitioned))));
            if(ibp.has_inserted)
                worklist.emplace_back(ibp.iterator);
            fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[ibp.iterator->item_set.accessing_symbol()].name_or_alias().c_str(),ibp.iterator->index,ibp.iterator->index);
            if(fp_log) fprintf(fp_log,"GOTO(%zu,%s)=%zu\n",current_state->index,token_datas[ibp.iterator->item_set.accessing_symbol()].name_or_alias().c_str(),ibp.iterator->index);
            ++current_transitioned;
        }
        for(;current_transitioned!=transitions_end;++current_transitioned){
            if(!current_actions.empty_reductions.resolve_rs_conflict({fp_log,token_datas,rs_conflicts,current_state->index},current_transitioned->accessing_symbol(),token_datas[current_transitioned->accessing_symbol()]))
                continue;
            const yuki::IB_Pair<typename Set<CLR1_State>::const_iterator> ibp = states.template insert<Equal>(CLR1_Item_Set<Token_Kind_t>(std::move(yuki::as_non_const(*current_transitioned))));
            if(ibp.has_inserted)
                worklist.emplace_back(ibp.iterator);
            fprintf(fp_file,IND "case %s: p_.SHIFT_(std::move(t_),%zu); t_=l_.lex(); goto S%zu;\n",token_datas[ibp.iterator->item_set.accessing_symbol()].name_or_alias().c_str(),ibp.iterator->index,ibp.iterator->index);
            if(fp_log) fprintf(fp_log,"GOTO(%zu,%s)=%zu\n",current_state->index,token_datas[ibp.iterator->item_set.accessing_symbol()].name_or_alias().c_str(),ibp.iterator->index);
            current_admissible_terms.push_back(ibp.iterator->item_set.accessing_symbol());
        }
        print_reductions:
        for(const Reduction<Token_Kind_t> reduction : current_actions.empty_reductions){
            if(reduction.valid()){
                fprintf(fp_file,IND "case %s: p_.SET_STATE(%zu); return {%s,%zu};\n",token_datas[reduction.term].name_or_alias().c_str(),reduction.rule,token_datas[reduction.term].name_or_alias().c_str(),reduction.rule);
                reducible[reduction.rule].reducible=true;
                current_admissible_terms.push_back(reduction.term);
            }
        }
        fprintf(fp_file,IND "default: return {(size_t)-1,(size_t)-1};\n} // case %zu\n",current_state->index);
        current_actions.clear();
        admissible_terms.emplace_back(std::move(current_admissible_terms));
    } // while(!worklist.empty())

    first_table.free();
    delete[] lr1_ias;

    for(size_t i=1;i<total_rules;++i)
        if(!reducible[i].reducible)
            fprintf(fp_err,"Warning: Non-reducible production %zu (%s) !\n",i,token_datas[reducible[i].left].name_or_alias().c_str());

    if(rs_conflicts || rr_conflicts)
        fprintf(fp_err,"%zu RS-conflict(s), %zu RR-conflict(s) in total. See log for details.\n",rs_conflicts,rr_conflicts);

    return states.size()+1;
} // write_clr1




template<std::unsigned_integral Token_Kind_t>
void print_rule(FILE* const fp,const Rule<Token_Kind_t>& r,const Token_Datas& token_datas){
    fprintf(fp,"%s ->",
        r.left!=(Token_Kind_t)-1 ? token_datas[r.left].name_or_alias().c_str() : "Goal_"
    );
    for(const Token_Kind_t k : r.rights)
        fprintf(fp," %s",token_datas[k].name_or_alias().c_str());
}

std::string print_rule_buf;

template<std::unsigned_integral Token_Kind_t>
void print_rule_escaped(FILE* const fp,const Rule<Token_Kind_t>& r,const Token_Datas& token_datas){
    if(r.left!=(Token_Kind_t)-1){
        const std::string_view noa = token_datas[r.left].name_or_alias();
        for(const char c : noa){
            switch(c){
                case '\'': print_rule_buf.append("\\\'");break;
                case '\"': print_rule_buf.append("\\\"");break;
                case '\\': print_rule_buf.append("\\\\");break;
                default: print_rule_buf.push_back(c);break;
            }
        }
        fprintf(fp,"%s ->",print_rule_buf.c_str());
        print_rule_buf.clear();
    }else
        fputs("Goal_ ->",fp);

    for(const Token_Kind_t k : r.rights){
        const std::string_view noa = token_datas[k].name_or_alias();
        for(const char c : noa){
            switch(c){
                case '\'': print_rule_buf.append("\\\'");break;
                case '\"': print_rule_buf.append("\\\"");break;
                case '\\': print_rule_buf.append("\\\\");break;
                default: print_rule_buf.push_back(c);break;
            }
        }
        fprintf(fp," %s",print_rule_buf.c_str());
        print_rule_buf.clear();
    }
}


template<std::unsigned_integral Token_Kind_t>
void write_parse_switch(FILE* const out,const Sec0_Data& sec0_data,const Rule_Set<Token_Kind_t>& rules){
    fprintf(out,
        "int %s::parse(%s& l_){\n"
        IND "reset();\n"
        IND "stack_.reserve(%zu);\n"
        IND "Action_Table action_table(l_);\n"
        "\n"
        IND "while(true){\n"
        IND2 "yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);\n"
        IND2 "switch(art_.rule){\n",
        sec0_data.parser.c_str(), sec0_data.lexer.c_str(),
        sec0_data.lr1_stack_reserve
    );

    for(const Rule<Token_Kind_t>& rule : rules){
        fprintf(out, IND3 "case %zu:{ // ",rule.num);
        print_rule(out,rule,sec0_data.token_datas);
        fprintf(out,
            "\n"
            IND4 "assert(stack_.size()>=%zu);\n"
            IND4 "const size_t start_ = stack_.size()-%zu;\n",
            rule.rights.size(), rule.rights.size()
        );
        switch(sec0_data.token_impl_type){
            case Sec0_Data::Token_Impl_Type::VARIANT:{
                for(size_t i = 0;i<rule.rights.size();++i){
                    const Token_Data& td = sec0_data.token_datas[rule.rights[i]];
                    if(!td.types.empty())
                        fprintf(out,
                            IND4 "%s& token%zu_ = stack_[start_+%zu].token.get<%zu>();\n",
                            td.types[0].c_str(),i,i,td.vtoken_index
                        );
                }
                if(rule.left!=(Token_Kind_t)-1){
                    const Token_Data& td = sec0_data.token_datas[rule.left];
                    fprintf(out,IND4 "Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::%s>,",td.name.c_str());
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end}",rule.rights.size()-1);
                    else
                        fputs("{}",out);
                    fprintf(out,"%s%s);\n", rule.init.empty() ? "" : ",", rule.init.c_str());
                    if(!td.types.empty())
                        fprintf(out,IND4 "%s& token_target_ = token_target_complete_.get<%zu>();\n\n",td.types[0].c_str(),td.vtoken_index);
                }
                break;
            }
            case Sec0_Data::Token_Impl_Type::SIMPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,IND4 "Token_t& token%zu_ = stack_[start_+%zu].token;\n", i,i);
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND4 "Token_t token_target_{Token_Kind::%s,%s",
                        sec0_data.token_datas[rule.left].name.c_str(),rule.init.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,",{token0_.location_range.begin,token%zu_.location_range.end}};\n\n", rule.rights.size()-1);
                    else
                        fputs("};\n\n",out);
                }
                break;
            }
            case Sec0_Data::Token_Impl_Type::TUPLE:{
                for(size_t i = 0;i<rule.rights.size();++i)
                    fprintf(out,
                        IND4 "Token::%s& token%zu_ = stack_[start_+%zu].token.get<Token::%s>();\n",
                        sec0_data.token_datas[rule.rights[i]].name.c_str(), i, i, sec0_data.token_datas[rule.rights[i]].name.c_str()
                    );
                if(rule.left!=(Token_Kind_t)-1){
                    fprintf(out,
                        IND4 "auto p_token_target_ = Token_t::alloc.template allocate<Token::%s>();\n"
                        IND4 "yuki::pg::Location_Range loc_target_ = ",
                        sec0_data.token_datas[rule.left].name.c_str()
                    );
                    if(rule.rights.size()!=0)
                        fprintf(out,"{stack_[start_].token.location_range().begin,stack_[start_+%zu].token.location_range().end};\n", rule.rights.size()-1);
                    else
                        fputs("{};\n",out);
                    fprintf(out,
                        IND4 "YUKI_CONSTRUCT_BRACE(p_token_target_,%s);\n"
                        IND4 "Token::%s& token_target_ = *p_token_target_;\n\n",
                        rule.init.c_str(),  sec0_data.token_datas[rule.left].name.c_str()
                    );
                }
                break;
            }
        }
        fprintf(out,
            IND4 "{%s\n"
            IND4 "}\n"
            "\n",
            rule.code.c_str()
        );
        if(sec0_data.is_tuple()){
            for(size_t i = rule.rights.size();i!=0;--i){
                fprintf(out, IND4 "stack_[start_+%zu].token.static_destroy_deallocate<Token::%s>();\n",
                    i-1, sec0_data.token_datas[rule.rights[i-1]].name.c_str()
                );
                fprintf(out,IND4 "stack_.pop_back();\n");
            }
        }else{
            fprintf(out,IND4 "stack_.pop_back(%zu);\n",rule.rights.size());
        }
        if(rule.left==(Token_Kind_t)-1){
            if(sec0_data.is_tuple())
                fprintf(out,IND4 "action_table.static_destroy_deallocate<Token::EOF_>();\n");
            fprintf(out,IND4 "return 0;\n" IND3 "}\n");
        }else{
            fprintf(out,
                IND4 "state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::%s);\n",
                sec0_data.token_datas[rule.left].name.c_str()
            );
            switch(sec0_data.token_impl_type){
                case Sec0_Data::Token_Impl_Type::VARIANT: fprintf(out,IND4 "stack_.emplace_back(std::move(token_target_complete_),state_);\n"); break;
                case Sec0_Data::Token_Impl_Type::SIMPLE: fprintf(out,IND4 "stack_.emplace_back(std::move(token_target_),state_);\n"); break;
                case Sec0_Data::Token_Impl_Type::TUPLE: fprintf(out,IND4 "stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);\n"); break;
            }
            fprintf(out,IND4 "YUKI_PG_DBGO(\"REDUCE ");
            print_rule_escaped(out,rule,sec0_data.token_datas);
            fprintf(out,
                "\\n\");\n"
                IND4 "break;\n"
                IND3 "} // case %zu //",
                rule.num
            );
            print_rule(out,rule,sec0_data.token_datas);
            fputc(static_cast<unsigned char>('\n'),out);
        }
    } // for(const Rule<Token_Kind_t>& rule : rules)
    fprintf(out,
        IND3 "default:{\n"
        IND4 "yuki::print_error(stderr,\"Syntax Error!\\n\");\n"
        IND4 "reset();\n"
        IND4 "yuki::print_error(stderr,\"Stack Clear!\\n\");\n"
        IND4 "return 1;\n"
        IND3 "}\n"
        IND2 "} // switch(art_.rule)\n"
        IND "} // while(true)\n"
        "} // int %s::parse(%s&)\n",
        sec0_data.parser.c_str(),sec0_data.lexer.c_str()
    );
} // void write_parse_switch


template<std::unsigned_integral Token_Kind_t>
void write(const Cmd_Data& cmd_data,const Sec0_Data& sec0_data,const Rule_Set<Token_Kind_t>& rules){

    FILE* const out = cmd_data.fp_out_cpp;
    FILE* const fp_log = cmd_data.fp_log;

    const typename Make_LR1_IAs<Token_Kind_t>::Ret lr1_ias_ret = Make_LR1_IAs<Token_Kind_t>(sec0_data.token_datas.nterms.size(),rules,fp_log,sec0_data.token_datas).calc();
    if(fp_log){
        if(cmd_data.ft){
            fprintf(fp_log,"\n\n\n\nFirst Table:\n%s\n",log_banner);
            print(fp_log,sec0_data.token_datas,lr1_ias_ret.first_table);
        }
        if(cmd_data.lr0_its){
            const LR0_ImmTransitions<Token_Kind_t>* const lr0_its = Make_LR0_ITs<Token_Kind_t>(sec0_data.token_datas.nterms.size(),rules).calc();
            fputs("\n\n\n\nLR0 Immediate Actions:\n",fp_log);
            print(fp_log,sec0_data.token_datas,lr0_its);
            delete[] lr0_its;
        }
        if(cmd_data.lr1_ias){
            fputs("\n\n\n\nLR1 Immediate Actions:\n",fp_log);
            print(fp_log,sec0_data.token_datas,lr1_ias_ret.immediate_actions,lr1_ias_ret.lookahead_proplists);
        }
        fputs("\n\n\n\n",fp_log);
    }

    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="cpp_top"){
            fputs(code.contents.c_str(),out);
            fputc(static_cast<unsigned char>('\n'),out);
            break;
        }
    }
    fprintf(out,
        "#include<yuki/print.hpp>\n"
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n"
        "\n"
        "%s\n"
        "yuki::pg::lr1_action_return_type %s::Action_Table::operator()(yuki::pg::AbsLR1Parser<%s>& p_,%s& l_){\n"
        "using enum %s::Token_Kind::enum_t;\n"
        "switch(p_.state()){\n",
        cmd_data.out_h.c_str(),
        sec0_data.nspace_head.c_str(),
        sec0_data.parser_tables.c_str(),sec0_data.ts.c_str(),sec0_data.lexer.c_str(),
        sec0_data.ts.c_str()
    );
    Reducible<Token_Kind_t>* const reducible = yuki::Allocator<Reducible<Token_Kind_t>>::allocate(rules.size());
    rules.recursive_traverse([reducible](const Rule<Token_Kind_t>& rule){::new(reducible+rule.num) Reducible<Token_Kind_t>{rule.left};});
    yuki::Vector<std::basic_string<Token_Kind_t>> admissible_terms;
    size_t state_size=0;
    switch(sec0_data.alg_type){
        case Sec0_Data::Alg_Type::NIL:{
            switch(cmd_data.alg_type){
                case Cmd_Data::Alg_Type::PLR1:
                    state_size = lr::write_lalr1<Weakly_Compatible,Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
                    break;
                case Cmd_Data::Alg_Type::CLR1:
                    state_size = lr::write_clr1<Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
                    break;
                case Cmd_Data::Alg_Type::LALR1:
                    state_size = lr::write_lalr1<IsoCore,Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
                    break;
            }
            break;
        }
        case Sec0_Data::Alg_Type::PLR1:
            state_size = lr::write_lalr1<Weakly_Compatible,Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
            break;
        case Sec0_Data::Alg_Type::CLR1:
            state_size = lr::write_clr1<Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
            break;
        case Sec0_Data::Alg_Type::LALR1:
            state_size = lr::write_lalr1<IsoCore,Token_Kind_t>(cmd_data.fp_out_cpp,stderr,fp_log,sec0_data.token_datas,lr1_ias_ret,reducible,rules.size(),admissible_terms);
            break;
    }
    static_assert(std::is_trivially_destructible_v<Reducible<Token_Kind_t>>);
    yuki::Allocator<Reducible<Token_Kind_t>>::deallocate(reducible,rules.size());
    fprintf(out,
        "default: return {(size_t)-1,(size_t)-1};\n"
        "} // switch(p_.state())\n"
        "} // yuki::pg::lr1_action_return_type %s::Action_Table::operator()(yuki::pg::AbsLR1Parser<%s>& p_,%s& l_)\n"
        "\n"
        "\n",
        sec0_data.parser_tables.c_str(),sec0_data.ts.c_str(),sec0_data.lexer.c_str()
    );
    fprintf(out,"constinit %s::Goto_Table %s::goto_table = {\n",sec0_data.parser_tables.c_str(),sec0_data.parser_tables.c_str());
    fprintf(out,
        "}; // constinit %s::Goto_Table %s::goto_table\n"
        "\n"
        "\n",
        sec0_data.parser_tables.c_str(),sec0_data.parser_tables.c_str()
    );
    write_parse_switch(out,sec0_data,rules);
    fputs(sec0_data.nspace_tail.c_str(),out);
    fputs("\n\n",out);
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="SEC2_"){
            fprintf(out,"%s\n",code.contents.c_str());
            break;
        }
    }


    FILE* const out_h = cmd_data.fp_out_h;
    fputs("#pragma once\n",out_h);
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="h_top"){
            fputs(code.contents.c_str(),out_h);
            fputc(static_cast<unsigned char>('\n'),out_h);
            break;
        }
    }
    fprintf(out_h,
        "#include<yuki/pg/lr1.hpp>\n"
        "#include\"%s\"\n"
        "\n"
        "#ifdef %s\n"
        "#include<yuki/print.hpp>\n"
        "#ifndef %s_LOG\n"
        "#define %s_LOG \"%s.log\"\n"
        "#endif\n"
        "%s\n"
        "#ifndef %s_FP\n"
        "inline FILE* const %s_fp_=fopen(%s_LOG,\"w\");\n"
        "#define %s_FP %s::%s_fp_\n"
        "#endif\n"
        "%s\n"
        "#define %sO(...) yuki::dbgout_base(fp_dbg_,\"%s\",__VA_ARGS__)\n"
        "#define CONSTEXPR_%s // Debug output would render constexpr-functions non-constexpr.\n"
        "#else\n"
        "#define %sO(...)\n"
        "#define CONSTEXPR_%s constexpr\n"
        "#endif\n"
        "\n",
        cmd_data.out_token.c_str(),
        sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),sec0_data.debug_prefix.c_str(),
        sec0_data.nspace_head.c_str(),
        sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),sec0_data.nspace.c_str(),sec0_data.debug_prefix.c_str(),
        sec0_data.nspace_tail.c_str(),
        sec0_data.debug_prefix.c_str(),sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str(),
        sec0_data.debug_prefix.c_str()
    );
    fprintf(out_h,
        "%s\n"
        "struct %s{\n"
        IND "struct Action_Table{\n"
        IND YUKI_PG_HIND "private:\n"
        IND2 "%s::Token_t t_;\n"
        IND YUKI_PG_HIND "public:\n"
        IND2 "Action_Table(%s& l_) noexcept : t_(l_.lex()) {}\n"
        IND2 "yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<%s>&,%s&);\n",
        sec0_data.nspace_head.c_str(),
        sec0_data.parser_tables.c_str(),
        sec0_data.ts.c_str(),
        sec0_data.lexer.c_str(),
        sec0_data.ts.c_str(),sec0_data.lexer.c_str()
    );
    if(sec0_data.is_tuple())
        fprintf(out_h,
            "\n"
            IND2 "template<typename T> requires %s::is_token_v<T>\n"
            IND "void static_destroy_deallocate() noexcept {t_.static_destroy_deallocate<T>();}\n",
            sec0_data.ts.c_str()
        );
    fprintf(out_h,
        IND "};\n"
        IND "typedef yuki::pg::LR1_Goto_Table<%s,%zu> Goto_Table;\n"
        IND "static constinit Goto_Table goto_table;\n"
        "};\n"
        "\n",
        sec0_data.ts.c_str(), state_size
    );
    fprintf(out_h,
        "struct %s %s: yuki::pg::AbsLR1Parser<%s>, private %s {\n"
        IND "%s* lexer;\n"
        "\n"
        IND "using %s::Action_Table;\n"
        IND "using %s::Goto_Table;\n"
        IND "using %s::goto_table;\n"
        "\n",
        sec0_data.parser.c_str(), sec0_data.no_final_class ? "" : "final ", sec0_data.ts.c_str(), sec0_data.parser_tables.c_str(),
        sec0_data.lexer.c_str(),
        sec0_data.parser_tables.c_str(),
        sec0_data.parser_tables.c_str(),
        sec0_data.parser_tables.c_str()
    );
    if(!sec0_data.no_default_ctor){
        fprintf(out_h,
            IND "constexpr %s() noexcept = default;\n"
            IND "explicit constexpr %s(%s* const l) noexcept : lexer(l) {}\n\n",
            sec0_data.parser.c_str(),
            sec0_data.parser.c_str(),sec0_data.lexer.c_str()
        );
    }
    fprintf(out_h,
        IND "int parse(%s&);\n"
        "\n"
        IND "virtual int parse() override %s{assert(lexer); return parse(*lexer);}\n"
        "\n",
        sec0_data.lexer.c_str(),
        sec0_data.no_final_function ? "" : "final "
    );
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="class"){
            fprintf(out_h,"%s\n",code.contents.c_str());
            break;
        }
    }
    fprintf(out_h,"\n}; // struct %s\n%s\n",sec0_data.parser.c_str(),sec0_data.nspace_tail.c_str());
    for(const Sec0_Data::Code& code : sec0_data.codes){
        if(code.qualifier=="h_bottom"){
            fprintf(out_h,"%s\n",code.contents.c_str());
            break;
        }
    }
} // write


template void write<unsigned char>(const Cmd_Data&,const Sec0_Data&,const Rule_Set<unsigned char>&);
template void write<unsigned short>(const Cmd_Data&,const Sec0_Data&,const Rule_Set<unsigned short>&);
template void write<unsigned>(const Cmd_Data&,const Sec0_Data&,const Rule_Set<unsigned>&);
} // namespace yuki::pg::lr
