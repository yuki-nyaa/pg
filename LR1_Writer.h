#pragma once
#include<concepts>
#include<compare>
#include<string>
#include<unordered_map>
#include<fmt/core.h>
#include<yuki/Map.hpp>
#include<yuki/Vector.hpp>
#include<yuki/RingQueue.hpp>
#include"common.hpp"

namespace yuki::pg{

struct Cmd_Data; // Forward declaration required for `LR1_Writer::write`.

template<std::unsigned_integral Token_Kind_t>
struct LR1_Writer{
    struct LR1_Item_Head{
        Token_Kind_t cursored;
        Token_Kind_t left;

        constexpr bool is_complete() const {return cursored==Token_Kind_t(-1);}
        friend constexpr std::strong_ordering operator<=>(LR1_Item_Head,LR1_Item_Head) noexcept = default;
    };

    struct LR1_Item_Body_Key{
        size_t rights_size;
        Token_Kind_t lookahead;
        size_t cursor;

        friend std::strong_ordering operator<=>(LR1_Item_Body_Key,LR1_Item_Body_Key) = default;
    };

    struct LR1_Item_Body{
        std::basic_string<Token_Kind_t> rights;
        Token_Kind_t lookahead;
        size_t cursor;

        template<typename R>
        LR1_Item_Body(R&& rs,const Token_Kind_t lh,const size_t c) noexcept :
            rights(std::forward<R>(rs)),
            lookahead(lh),
            cursor(c)
        {}

        struct To_Key{
            LR1_Item_Body_Key operator()(const LR1_Item_Body& ib) const {return {ib.rights.size(),ib.lookahead,ib.cursor};}
        };

        friend bool operator==(const LR1_Item_Body& lhs,const LR1_Item_Body& rhs){
            return lhs.lookahead==rhs.lookahead && lhs.cursor==rhs.cursor && lhs.rights==rhs.rights;
        }
    };

    struct LR1_Item_Set_Set;

    struct LR1_Item_Set{
        typedef yuki::RB_Tree<
            LR1_Item_Body_Key,
            LR1_Item_Body,
            typename LR1_Item_Body::To_Key,
            yuki::Less<LR1_Item_Body_Key>,
            yuki::Allocator<yuki::RB_Tree_Node<LR1_Item_Body>>
        > block_type;
        typedef yuki::Map<LR1_Item_Head,block_type> map_type;
      private:
        map_type map_;
        size_t size_ = 0;
      public:
        constexpr LR1_Item_Set() noexcept = default;
        LR1_Item_Set(const LR1_Item_Set&) noexcept = default;

        constexpr LR1_Item_Set(LR1_Item_Set&& other) noexcept :
            map_(std::move(other.map_)),
            size_(other.size_)
        {other.size_=0;}

        /// @note If there are multiple goal productions, only the first is considered.
        LR1_Item_Set(const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable) noexcept {
            typename Rule_Set<Token_Kind_t>::const_iterator b = rules.begin(-1); // `-1` is the number of "Goal_".
            const typename Rule_Set<Token_Kind_t>::const_iterator e = rules.end();
            for(;b!=e;++b)
                emplace(-1,std::initializer_list<Token_Kind_t>{b->rights.front()},ftable.eof(),0);
            closure(rules,ftable);
        }

        struct const_iterator{
            friend LR1_Item_Set;
          private:
            typename map_type::const_iterator x;
            typename map_type::const_iterator x_end;
            typename block_type::const_iterator y;
            constexpr const_iterator(const typename map_type::const_iterator xp,const typename map_type::const_iterator xep) noexcept:
                x(xp),x_end(xep)
            {
                if(x!=x_end)
                    y=x->mapped.begin();
            }
            constexpr const_iterator(const typename map_type::const_iterator xp,const typename map_type::const_iterator xep,const typename block_type::const_iterator yp) noexcept:
                x(xp),x_end(xep),y(yp)
            {}
          public:
            Token_Kind_t left() const {return x->key.left;}
            const std::basic_string<Token_Kind_t>& rights() const {return y->rights;}
            Token_Kind_t lookahead() const {return y->lookahead;}
            size_t cursor() const {return y->cursor;}
            Token_Kind_t cursored() const {return x->key.cursored;}
            bool is_complete() const {return x->key.is_complete();}

            const_iterator& operator++(){
                ++y;
                if(y==x->mapped.end()){
                    ++x;
                    if(x!=x_end)
                        y=x->mapped.begin();
                }
                return *this;
            }

            constexpr bool is_end() const {return x==x_end;}

            friend constexpr bool operator==(const const_iterator lhs,const const_iterator rhs){
                return lhs.x==rhs.x && (lhs.x==lhs.x_end || lhs.y==rhs.y);
            }

            friend bool body_equal(const const_iterator lhs,const const_iterator rhs) {return *(lhs.y)==*(rhs.y);}
        };

        const_iterator begin() const {return {map_.begin(),map_.end()};}

        constexpr bool empty() const {return size_==0;}
        constexpr size_t size() const {return size_;}
        constexpr size_t block_count() const {return map_.size();}

        void clear() {map_.clear(); size_=0;}

        yuki::IB_Pair<const_iterator> emplace(const Token_Kind_t l,const std::basic_string<Token_Kind_t>& rs,const Token_Kind_t lh,const Token_Kind_t c){
            const yuki::IB_Pair<typename map_type::iterator> result1 = map_.template emplace<true>({(c==rs.size() ? Token_Kind_t(-1) : rs[c]), l});
            if(!result1.has_inserted){
                typename block_type::const_iterator feg = result1.iterator->mapped.first_equiv_greater({rs.size(),lh,c});
                const typename block_type::const_iterator fg = result1.iterator->mapped.first_greater({rs.size(),lh,c});
                for(;feg!=fg;++feg)
                    if(feg->rights==rs)
                        return {{result1.iterator,map_.end(),feg},false};
            }
            ++size_;
            return {{result1.iterator,map_.end(),result1.iterator->mapped.emplace(rs,lh,c)},true};
        }

        yuki::IB_Pair<const_iterator> emplace(const Token_Kind_t l,std::basic_string<Token_Kind_t>&& rs,const Token_Kind_t lh,const Token_Kind_t c){
            const yuki::IB_Pair<typename map_type::iterator> result1 = map_.template emplace<true>({c==rs.size() ? Token_Kind_t(-1) : rs[c],l});
            if(!result1.has_inserted){
                typename block_type::const_iterator feg = result1.iterator->mapped.first_equiv_greater({rs.size(),lh,c});
                const typename block_type::const_iterator fg = result1.iterator->mapped.first_greater({rs.size(),lh,c});
                for(;feg!=fg;++feg)
                    if(feg->rights==rs)
                        return {{result1.iterator,map_.end(),feg},false};
            }
            ++size_;
            return {{result1.iterator,map_.end(),result1.iterator->mapped.emplace(std::move(rs),lh,c)},true};
        }

        struct Less_By_Size{
            static std::strong_ordering compare3(const LR1_Item_Set& lhs,const LR1_Item_Set& rhs){
                if(const std::strong_ordering cmp=lhs.size_<=>rhs.size_; cmp!=0) return cmp;
                if(const std::strong_ordering cmp=lhs.map_.size()<=>rhs.map_.size(); cmp!=0) return cmp;
                return std::strong_ordering::equal;
            }
            bool operator()(const LR1_Item_Set& lhs,const LR1_Item_Set& rhs) const {return compare3(lhs,rhs)==std::strong_ordering::less;}
        };

        friend LR1_Item_Set_Set;

        void closure(const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable){
            #ifndef YUKI_PG_LR1_CLOSURE_WORKLIST_RESERVE
            #define YUKI_PG_LR1_CLOSURE_WORKLIST_RESERVE 4096
            #endif
            static yuki::RingQueue<typename LR1_Item_Set::const_iterator> worklist(yuki::reserve_tag,YUKI_PG_LR1_CLOSURE_WORKLIST_RESERVE);

            worklist.clear();
            worklist.reserve(size_);
            for(typename LR1_Item_Set::const_iterator b = begin();!b.is_end();++b)
                worklist.emplace_back(b);

            while(!worklist.empty()){
                const typename LR1_Item_Set::const_iterator item = worklist.pop_front_v();
                if(!item.is_complete() && item.cursored()<ftable.term_first() ){ // `item.cursored()` is nterminal.
                    typename Rule_Set<Token_Kind_t>::const_iterator b = rules.begin(item.cursored());
                    const typename Rule_Set<Token_Kind_t>::const_iterator e = rules.end(item.cursored());
                    if(b!=rules.end()){
                        const Token_Kind_t item_lookahead=item.lookahead();
                        const std::basic_string_view<Token_Kind_t> item_rights(item.rights());
                        const size_t item_cursor = item.cursor();
                        for(;b!=e;++b){
                            typename First_Table<Token_Kind_t>::extended_const_iterator_ne e_i=ftable.begin_ne(item_rights.substr(item_cursor+1),item_lookahead);
                            for(;!e_i.is_end();++e_i){
                                const yuki::IB_Pair<typename LR1_Item_Set::const_iterator> ibp = emplace(item_rights[item_cursor],b->rights,*e_i,0);
                                if(ibp.has_inserted)
                                    worklist.emplace_back(ibp.iterator);
                            }
                        }
                    }
                }
            }

        }

        // Not used in this program.
        LR1_Item_Set go_to(const Token_Kind_t kind,const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable) const {
            LR1_Item_Set moved;
            typename LR1_Item_Set::const_iterator item = begin();
            for(;!item.is_end();++item){
                if(!item.is_complete() && item.cursored()==kind)\
                    moved.emplace(item.left(),item.rights(),item.lookahead(),item.cursor()+1);
            }
            moved.closure(rules,ftable);
            return moved;
        }
    }; // struct LR1_Item_Set




    struct LR1_Item_Set_Set{
        typedef yuki::MultiMap<LR1_Item_Set,size_t,typename LR1_Item_Set::Less_By_Size> map_type;
      private:
        map_type map_;
      public:
        typedef typename map_type::const_iterator const_iterator;

        const_iterator begin() const {return map_.begin();}
        const_iterator end() const {return map_.end();}

        size_t size() const {return map_.size();}

        yuki::IB_Pair<const_iterator> insert(LR1_Item_Set&& items){
            const_iterator feg = map_.first_equiv_greater(items);
            const const_iterator fg = map_.first_greater(items);
            for(;feg!=fg;++feg){
                {
                typename LR1_Item_Set::map_type::const_iterator lb = feg->key.map_.begin();
                typename LR1_Item_Set::map_type::const_iterator rb = items.map_.begin();
                const typename LR1_Item_Set::map_type::const_iterator le = feg->key.map_.end();
                for(;lb!=le;++lb,++rb)
                    if(lb->key!=rb->key || lb->mapped.size()!=rb->mapped.size())
                        goto loop_end;
                }
                {
                typename LR1_Item_Set::const_iterator lb = feg->key.begin();
                typename LR1_Item_Set::const_iterator rb = items.begin();
                for(;!lb.is_end();++lb,++rb)
                    if(!body_equal(lb,rb))
                        goto loop_end;
                }

                items.clear();
                return {feg,false};

                loop_end:;
            }
            return {map_.emplace(std::move(items),map_.size()),true};
        }
    };




    struct Action_Candidates{ // Note that this struct is `memset`able.
        size_t shift; // 0 represents the null value. The state 0 (i.e. the initial state) is actually un-goto-able, so this is fine.
        struct{
        size_t rule_num;
        prec_t prec_sr;
        prec_t prec_rr;
        } reduce; // 0 represents the null value of `rule_num`. Of course I have to make sure that no rule has number 0.

        enum struct State {EMPTY,S,R,SR};
        constexpr State state() const noexcept {
            if(shift!=0)
                return reduce.rule_num!=0 ? State::SR : State::S;
            else
                return reduce.rule_num!=0 ? State::R : State::EMPTY;
        }
        constexpr bool empty() const noexcept {return shift==0 && reduce.rule_num==0;}
    };

    static_assert(std::is_trivial_v<Action_Candidates>);
    static_assert(std::is_standard_layout_v<Action_Candidates>);

    static void resolve_rr_conflict(Action_Candidates& ac1,const size_t rule_num_2,const prec_t prec_sr_2,const prec_t prec_rr_2){
        // First select the rule with a higher rr-precedence; if same, then select the rule that has the smaller number, i.e. the rule that appears earlier in the grammar specs.
        // Note that `prec_sr` does not participate in the resolution process. It is recored for later SR-conflict resolution.
        if( (ac1.reduce.prec_rr<prec_rr_2) || ((ac1.reduce.prec_rr==prec_rr_2) && (ac1.reduce.rule_num>rule_num_2)) )
            ac1.reduce={rule_num_2,prec_sr_2,prec_rr_2};
    }

    enum struct Action_Kind {SHIFT,REDUCE};

    static Action_Kind resolve_sr_conflict(const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,const Assoc assoc0,const Action_Candidates& ac,const unsigned long long lookahead){
        auto get_prec = [&nterms,&terms](const unsigned long long k) -> prec_t
        {
            return (k>=nterms.size() ? terms[k-nterms.size()] : nterms[k]).prec;
        };
        auto get_assoc = [&nterms,&terms](const unsigned long long k) -> Assoc
        {
            return (k>=nterms.size() ? terms[k-nterms.size()] : nterms[k]).assoc;
        };

        if(ac.reduce.prec_sr<get_prec(lookahead)){
            return Action_Kind::SHIFT;
        }else if(ac.reduce.prec_sr>get_prec(lookahead)){
            return Action_Kind::REDUCE;
        }else{
            switch(ac.reduce.prec_sr==0 ? assoc0 : get_assoc(lookahead)){
                case Assoc::RIGHT : return Action_Kind::SHIFT;
                case Assoc::LEFT : return Action_Kind::REDUCE;
                default : return Action_Kind::SHIFT; // Unreachable
            }
        }
    }

    static size_t write_table(
        const bool is_switch,
        const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
        const Rule_Set<Token_Kind_t>& rules,
        const Assoc assoc0,
        FILE* const fp_file,FILE* const fp_goto,FILE* const fp_err,FILE* const fp_log);

    static void write_parse_array(
        FILE* const out,const Options& options,
        const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
        const Rule_Set<Token_Kind_t>& rules);

    static void write_parse_switch(
        FILE* const out,const Options& options,
        const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
        const Rule_Set<Token_Kind_t>& rules);

    static void write(
        const Cmd_Data& cmd_data,const Options& options,
        const std::unordered_map<std::string,std::string> code_htable,
        const yuki::Vector<Token_Data>& nterms,const yuki::Vector<Token_Data>& terms,
        const Rule_Set<Token_Kind_t>& rules);
}; // struct LR1_Writer<Token_Kind_t>
} // namespace yuki::pg
