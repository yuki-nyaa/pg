#pragma once
#include"common.hpp"
#include<concepts>
#include<string>

#include<map>
#include<vector>
#include<cstring>
#include<fmt/core.h>
namespace yuki::pg{
namespace lr1{}
namespace lalr1 = lr1;
}

namespace yuki::pg::lr1{
template<std::unsigned_integral Token_Kind_t>
struct LR1_Item{
    Token_Kind_t left;
    std::basic_string<Token_Kind_t> rights;
    Token_Kind_t lookahead;
    size_t cursor; // To the left of the indexed token-kind.
    mutable bool processed=false;

    bool is_complete() const noexcept {return cursor==rights.size();}

    friend bool operator==(const LR1_Item& lhs, const LR1_Item& rhs) noexcept {
        return lhs.lookahead==rhs.lookahead && lhs.left==rhs.left && lhs.rights.size()==rhs.rights.size() && lhs.cursor==rhs.cursor && lhs.rights==rhs.rights;
    }
};
}

#ifdef YUKI_PG_PRINT
template<std::unsigned_integral Token_Kind_t>
struct fmt::formatter<yuki::pg::lr1::LR1_Item<Token_Kind_t>,char> : yuki::simple_formatter<yuki::pg::lr1::LR1_Item<Token_Kind_t>,char> {
    template<typename OutputIt>
    auto format(const yuki::pg::lr1::LR1_Item<Token_Kind_t>& item,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
        typename fmt::basic_format_context<OutputIt,char>::iterator o = ctx.out();
        fmt::format_to(o,"({} -> )",static_cast<unsigned long long>(item.left));
        for(const Token_Kind_t i : item.rights)
            fmt::format_to(o,"{},",static_cast<unsigned long long>(i));
        return fmt::format_to(o," | {},{}",static_cast<unsigned long long>(item.lookahead),static_cast<unsigned long long>(item.cursor));
    }
};
#endif

namespace yuki::pg::lr1{
template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Char_1{
    Token_Kind_t cursored;
    Token_Kind_t left;
    explicit LR1_Item_Char_1(const LR1_Item<Token_Kind_t>& item) noexcept :
        cursored( (item.cursor==item.rights.size()) ? (Token_Kind_t(-1)) : (item.rights[item.cursor]) ),
        left(item.left)
    {}
    friend auto operator<=>(const LR1_Item_Char_1&,const LR1_Item_Char_1&) noexcept = default;
};

template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Char_2{
    size_t rights_size;
    Token_Kind_t lookahead;
    size_t cursor;
    explicit LR1_Item_Char_2(const LR1_Item<Token_Kind_t>& item) noexcept :
        rights_size(item.rights.size()),
        lookahead(item.lookahead),
        cursor(item.cursor)
    {}
    friend auto operator<=>(const LR1_Item_Char_2&,const LR1_Item_Char_2&) noexcept = default;
};

template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Simple{
    std::basic_string<Token_Kind_t> rights;
    mutable bool processed=false;
    LR1_Item_Simple(const LR1_Item<Token_Kind_t>& item) : rights(item.rights) {}
    LR1_Item_Simple(LR1_Item<Token_Kind_t>&& item) : rights(std::move(item.rights)) {}
};

template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Set{
  public:
    typedef std::vector<LR1_Item_Simple<Token_Kind_t>> bucket_type;
    typedef std::map<LR1_Item_Char_2<Token_Kind_t>,bucket_type> map_2_type;
    typedef typename map_2_type::value_type map_2_value_type;
    typedef std::map<LR1_Item_Char_1<Token_Kind_t>,map_2_type> map_type;
    typedef typename map_type::value_type map_value_type;
  private:
    map_type map_;
    size_t size_=0;
    size_t mid_size_=0;
  public:
    size_t reserve_size=10; // MNumber
    mutable bool processed=false;

    LR1_Item_Set()=default;

    LR1_Item_Set(LR1_Item_Set&& other) noexcept :
        map_(std::move(other.map_)),
        size_(other.size_),
        mid_size_(other.mid_size_),
        reserve_size(other.reserve_size),
        processed(other.processed)
    {
        assert(other.map_.empty()); // This should hold with most implementations. If it happens that your implementation does not make the moved-from container empty, say, the move constructor is actually implemented by copying, then I suggest you throw away this implementation and try another one.
        other.size_=0;other.mid_size_=0;
    };

    typedef typename map_type::iterator coarse_iterator;
    typedef typename map_type::const_iterator const_coarse_iterator;
    typedef typename map_type::reverse_iterator reverse_coarse_iterator;
    typedef typename map_type::const_reverse_iterator const_reverse_coarse_iterator;

    typedef typename map_2_type::iterator map_2_iterator;
    typedef typename map_2_type::const_iterator const_map_2_iterator;
    typedef typename map_2_type::reverse_iterator reverse_map_2_iterator;
    typedef typename map_2_type::const_reverse_iterator const_reverse_map_2_iterator;

    typedef typename bucket_type::iterator bucket_iterator;
    typedef typename bucket_type::const_iterator const_bucket_iterator;
    typedef typename bucket_type::reverse_iterator reverse_bucket_iterator;
    typedef typename bucket_type::const_reverse_iterator const_reverse_bucket_iterator;

    struct iterator;
    struct const_iterator;
    // Reverse-iterator is kinda stupid and largely useless IMO, so I am too lazy to write them.

    coarse_iterator coarse_begin() {return map_.begin();}
    const_coarse_iterator coarse_begin() const {return map_.begin();}
    const_coarse_iterator coarse_cbegin() {return map_.cbegin();}

    coarse_iterator coarse_end() {return map_.end();}
    const_coarse_iterator coarse_end() const {return map_.end();}
    const_coarse_iterator coarse_cend() {return map_.cend();}

    iterator begin(){
        coarse_iterator b1 = map_.begin();
        map_2_iterator b2 = (b1->second).begin();
        return iterator(b1,map_.end(),b2,(b2->second).begin());
    }
    const_iterator begin() const {
        const_coarse_iterator b1 = map_.begin();
        const_map_2_iterator b2 = (b1->second).begin();
        return const_iterator(b1,map_.end(),b2,(b2->second).begin());
    }
    const_iterator cbegin() const {
        const_coarse_iterator b1 = map_.cbegin();
        const_map_2_iterator b2 = (b1->second).cbegin();
        return const_iterator(b1,map_.cend(),b2,(b2->second).cbegin());
    }

    // I suggest using `iterator.is_end()`, unless you really want to use the stl algorithms.
    iterator end(){
        coarse_iterator b1 = map_.begin();
        map_2_iterator b2 = (b1->second).begin();
        return iterator(map_.end(),map_.end(),b2,(b2->second).begin());
    }
    const_iterator end() const {
        const_coarse_iterator b1 = map_.begin();
        const_map_2_iterator b2 = (b1->second).begin();
        return const_iterator(map_.end(),map_.end(),b2,(b2->second).begin());
    }
    const_iterator cend() const {
        const_coarse_iterator b1 = map_.cbegin();
        const_map_2_iterator b2 = (b1->second).cbegin();
        return const_iterator(map_.cend(),map_.cend(),b2,(b2->second).cbegin());
    }

    size_t coarse_size() const noexcept {return map_.size();}
    size_t mid_size() const noexcept {return mid_size_;}
    size_t size() const noexcept {return size_;}
    bool empty() const noexcept {return size_==0;};
    void clear(){map_.clear();mid_size_=0;size_=0;}

    iterator insert(const LR1_Item<Token_Kind_t>& item){
        auto [x,x_nexists] = map_.try_emplace(LR1_Item_Char_1<Token_Kind_t>(item),map_2_type{});
        if(x_nexists){
            map_2_iterator y = (x->second).emplace(LR1_Item_Char_2<Token_Kind_t>(item),bucket_type{}).first;
            (y->second).reserve(reserve_size);
            (y->second).emplace_back(item);
            ++size_;
            ++mid_size_;
            return iterator(x,map_.end(),y,(y->second).begin());
        }else{
            auto [y,y_nexists] = (x->second).try_emplace(LR1_Item_Char_2<Token_Kind_t>(item),bucket_type{});
            if(y_nexists){
                (y->second).reserve(reserve_size);
                (y->second).emplace_back(item);
                ++size_;
                ++mid_size_;
                return iterator(x,map_.end(),y,(y->second).begin());
            }else{
                bucket_iterator z = (y->second).begin();
                bucket_iterator z_end = (y->second).end();
                for(;z!=z_end;++z){
                    if(z->rights==item.rights)
                        return iterator(x,map_.end(),y,z);
                }
                ++size_;
                return iterator(x,map_.end(),y,(y->second).emplace((y->second).end(),item.rights));
            }
        }
    }
    iterator insert(LR1_Item<Token_Kind_t>&& item){
        auto [x,x_nexists] = map_.try_emplace(LR1_Item_Char_1<Token_Kind_t>(item),map_2_type{});
        if(x_nexists){
           map_2_iterator y = (x->second).emplace(LR1_Item_Char_2<Token_Kind_t>(item),bucket_type{}).first;
            (y->second).reserve(reserve_size);
            (y->second).emplace_back(std::move(item));
            ++size_;
            ++mid_size_;
            return iterator(x,map_.end(),y,(y->second).begin());
        }else{
            auto [y,y_nexists] = (x->second).try_emplace(LR1_Item_Char_2<Token_Kind_t>(item),bucket_type{});
            if(y_nexists){
                (y->second).reserve(reserve_size);
                (y->second).emplace_back(std::move(item));
                ++size_;
                ++mid_size_;
                return iterator(x,map_.end(),y,(y->second).begin());
            }else{
                bucket_iterator z = (y->second).begin();
                bucket_iterator z_end = (y->second).end();
                for(;z!=z_end;++z){
                    if(z->rights==item.rights)
                        return iterator(x,map_.end(),y,z);
                }
                ++size_;
                return iterator(x,map_.end(),y,(y->second).emplace((y->second).end(),std::move(item)));
            }
        }
    }
    //coarse_iterator insert_cursor_shifted(const_coarse_iterator hint,const map_value_type& block){
    //    map_2_type map_2{};
    //    map_2_iterator map_2_e = map_2.end();
    //    const_map_2_iterator it_2_b = (block.second).begin();
    //    const_map_2_iterator it_2_e = (block.second).end();
    //    for(;it_2_b!=it_2_e;++it_2_b){
    //        map_2.insert(map_2_e,*it_2_b);
    //        ++mid_size_;
    //        size_ += (it_2_b->second).size();
    //    }
    //    return map_.insert(hint,{block.first,std::move(map_2)});
    //}

    bool contains(const LR1_Item<Token_Kind_t>& item) const {
        const_coarse_iterator it_1= map_.find(LR1_Item_Char_1<Token_Kind_t>(item));
        if(it_1!=map_.end()){
            const_map_2_iterator it_2 = (it_1->second).find(LR1_Item_Char_2<Token_Kind_t>(item));
            if(it_2!=(it_1->second).end()){
                const_bucket_iterator first = (it_2->second).begin();
                const_bucket_iterator last = (it_2->second).end();
                for(;first!=last;++first) {
                    if(first->rights==item.rights) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    bool contains(const const_iterator& i,const_coarse_iterator hint_x) const {
        assert(hint_x!=map_.end());
        const_map_2_iterator it_2 = (hint_x->second).find((i.y)->first);
        if(it_2!=(hint_x->second).end()){
            const_bucket_iterator first = (it_2->second).begin();
            const_bucket_iterator last = (it_2->second).end();
            for(;first!=last;++first) {
                if(first->rights==i.rights()) {
                    return true;
                }
            }
        }
        return false;
    }

    friend bool operator==(const LR1_Item_Set& lhs,const LR1_Item_Set& rhs){
        if(lhs.size_==rhs.size_ && lhs.coarse_size()==rhs.coarse_size() && lhs.mid_size_==rhs.mid_size_){ // `mid_size_` is very likely the same as `size_`, so I compare it last.
            auto compare_1 = [](const map_value_type& lhs_1,const map_value_type& rhs_1)->bool{return (lhs_1.first==rhs_1.first) && (lhs_1.second.size()==rhs_1.second.size());};
            if(std::equal(lhs.map_.cbegin(),lhs.map_.cend(),rhs.map_.cbegin(),compare_1)){
                const_iterator it_lhs = lhs.begin();
                const_coarse_iterator it_x = rhs.map_.begin();
                for(;!it_lhs.is_end(); (it_lhs.increment_informative().first)?void(++it_x):void(0) ){
                    if(!rhs.contains(it_lhs,it_x))
                        return false;
                }
                return true;
            }
        }
        return false;
    }
}; // struct LR1_Item_Set

template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Set<Token_Kind_t>::iterator{
    friend struct LR1_Item_Set;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnon-template-friend"
    friend bool operator==(const LR1_Item_Set& lhs,const LR1_Item_Set& rhs);
    #pragma GCC diagnostic pop;
  private:
    coarse_iterator x;
    coarse_iterator x_end;
    map_2_iterator y;
    map_2_iterator y_begin;
    map_2_iterator y_end;
    bucket_iterator z;
    bucket_iterator z_begin;
    bucket_iterator z_end;
    iterator(
        const coarse_iterator& x_p,const coarse_iterator& x_end_p,
        const map_2_iterator& y_p,
        const bucket_iterator& z_p
    ) noexcept :
        x(x_p),x_end(x_end_p),
        y(y_p),y_begin((x->second).begin()),y_end((x->second).end()),
        z(z_p),z_begin((y->second).begin()),z_end((y->second).end())
    {}
    void z_to_begin(const map_2_iterator& y_p){
        z_begin=(y_p->second).begin();
        z_end=(y_p->second).end();
        z=z_begin;
    }
    void z_to_end(const map_2_iterator& y_p){
        z_begin=(y_p->second).begin();
        z_end=(y_p->second).end();
        z=z_end;
    }
    void y_to_begin(const coarse_iterator& x_p){
        y_begin=(x_p->second).begin();
        y_end=(x_p->second).end();
        y=y_begin;
    }
    void y_to_end(const coarse_iterator& x_p){
        y_begin=(x_p->second).begin();
        y_end=(x_p->second).end();
        y=y_end;
    }
  public:
    iterator(const coarse_iterator& x_p,const coarse_iterator& x_end_p) noexcept : iterator(x_p,x_end_p,(x->second).begin(),(y->second).begin()) {}
    iterator& operator++() noexcept {
        ++z;
        if(z==z_end){
            ++y;
            if(y!=y_end){
                z_to_begin(y);
            }else{
                ++x;
                if(x!=x_end){
                    y_to_begin(x);
                    z_to_begin(y);
                }
            }
        }
        return *this;
    }
    std::pair<bool,bool> increment_informative() noexcept { // Signals x and y change.
        ++z;
        if(z==z_end){
            ++y;
            if(y!=y_end){
                z_to_begin(y);
                return {false,true};
            }else{
                ++x;
                if(x!=x_end){
                    y_to_begin(x);
                    z_to_begin(y);
                }
                return {true,true};
            }
        }else{
            return {false,false};
        }
    }
    iterator& operator--() noexcept {
        if(z==z_begin){
            if(y==y_begin){
                --x;
                y_to_end(x);
                --y;
                z_to_end(y);
                --z;
            }else{
                --y;
                z_to_end(y);
                --z;
            }
        }else{
            --z;
        }
        return *this;
    }
    friend bool operator==(const iterator& lhs,const iterator& rhs) noexcept {
        return (lhs.x==rhs.x)
            && (
                (lhs.x==lhs.x_end) // Both are "end".
                || ( (lhs.y==rhs.y) && (lhs.z==rhs.z) ) // This should be enough.
            );
    }

    bool is_end() const noexcept {return x==x_end;}

    coarse_iterator get_x() const noexcept {return x;}
    map_2_iterator get_y() const noexcept {return y;}
    bucket_iterator get_z() const noexcept {return z;}

    void x_advance(){
        ++x;
        if(x!=x_end){
            y_to_begin(x);
            z_to_begin(y);
        }
    }
    void y_advance(){
        ++y;
        if(y!=y_end){
            z_to_begin(y);
        }
    }
    void z_reset(){z_to_begin(y);}

    Token_Kind_t left() const noexcept {return (x->first).left;}
    size_t rights_size() const noexcept {return (y->first).rights_size;}
    Token_Kind_t lookahead() const noexcept {return (y->first).lookahead;}
    size_t cursor() const noexcept {return (y->first).cursor;}
    Token_Kind_t cursored() const noexcept {return (x->first).cursored;}
    const std::basic_string<Token_Kind_t>& rights() const noexcept {return z->rights;}
    bool& processed() const noexcept {return z->processed;}
    bool is_complete() const noexcept {return cursored()==Token_Kind_t(-1);}
    const LR1_Item_Char_1<Token_Kind_t>& char1() const noexcept {return x->first;}
    const LR1_Item_Char_2<Token_Kind_t>& char2() const noexcept {return y->first;}
    #ifdef YUKI_PG_PRINT
    void print(FILE* fp){
    fmt::print(fp,"({} -> )",static_cast<unsigned long long>(left()));
    for(const Token_Kind_t i : z->rights)
        fmt::print(fp,"{},",static_cast<unsigned long long>(i));
    fmt::print(fp," | {},{}",static_cast<unsigned long long>(lookahead()),cursor());
    }
    #endif
}; // struct LR1_Item_Set<Token_Kind_t>::iterator

template<std::unsigned_integral Token_Kind_t>
struct LR1_Item_Set<Token_Kind_t>::const_iterator{
    friend struct LR1_Item_Set;
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wnon-template-friend"
    friend bool operator==(const LR1_Item_Set& lhs,const LR1_Item_Set& rhs);
    #pragma GCC diagnostic pop;
  private:
    const_coarse_iterator x;
    const_coarse_iterator x_end;
    const_map_2_iterator y;
    const_map_2_iterator y_begin;
    const_map_2_iterator y_end;
    const_bucket_iterator z;
    const_bucket_iterator z_begin;
    const_bucket_iterator z_end;
    const_iterator(
        const const_coarse_iterator& x_p,const const_coarse_iterator& x_end_p,
        const const_map_2_iterator& y_p,
        const const_bucket_iterator& z_p
    ) noexcept :
        x(x_p),x_end(x_end_p),
        y(y_p),y_begin((x->second).begin()),y_end((x->second).end()),
        z(z_p),z_begin((y->second).begin()),z_end((y->second).end())
    {}
    void z_to_begin(const const_map_2_iterator& y_p){
        z_begin=(y_p->second).begin();
        z_end=(y_p->second).end();
        z=z_begin;
    }
    void z_to_end(const const_map_2_iterator& y_p){
        z_begin=(y_p->second).begin();
        z_end=(y_p->second).end();
        z=z_end;
    }
    void y_to_begin(const const_coarse_iterator& x_p){
        y_begin=(x_p->second).begin();
        y_end=(x_p->second).end();
        y=y_begin;
    }
    void y_to_end(const const_coarse_iterator& x_p){
        y_begin=(x_p->second).begin();
        y_end=(x_p->second).end();
        y=y_end;
    }
  public:
    const_iterator(const const_coarse_iterator& x_p,const const_coarse_iterator& x_end_p) noexcept : const_iterator(x_p,x_end_p,(x->second).begin(),(y->second).begin()) {}
    const_iterator(const typename LR1_Item_Set<Token_Kind_t>::iterator& it) : x(it.x),x_end(it.x_end),y(it.y),y_begin(it.y_begin),y_end(it.y_end),z(it.z),z_begin(it.z_begin),z_end(it.z_end) {}
    const_iterator& operator++() noexcept {
        ++z;
        if(z==z_end){
            ++y;
            if(y!=y_end){
                z_to_begin(y);
            }else{
                ++x;
                if(x!=x_end){
                    y_to_begin(x);
                    z_to_begin(y);
                }
            }
        }
        return *this;
    }
    std::pair<bool,bool> increment_informative() noexcept { // Signals x and y change.
        ++z;
        if(z==z_end){
            ++y;
            if(y!=y_end){
                z_to_begin(y);
                return {false,true};
            }else{
                ++x;
                if(x!=x_end){
                    y_to_begin(x);
                    z_to_begin(y);
                }
                return {true,true};
            }
        }else{
            return {false,false};
        }
    }
    const_iterator& operator--() noexcept {
        if(z==z_begin){
            if(y==y_begin){
                --x;
                y_to_end(x);
                --y;
                z_to_end(y);
                --z;
            }else{
                --y;
                z_to_end(y);
                --z;
            }
        }else{
            --z;
        }
        return *this;
    }
    friend bool operator==(const const_iterator& lhs,const const_iterator& rhs) noexcept {
        return (lhs.x==rhs.x)
            && (
                (lhs.x==lhs.x_end) // Both are "end".
                || ( (lhs.y==rhs.y) && (lhs.z==rhs.z) ) // This should be enough.
            );
    }

    bool is_end() const noexcept {return x==x_end;}

    const_coarse_iterator get_x() const noexcept {return x;}
    const_map_2_iterator get_y() const noexcept {return y;}
    const_bucket_iterator get_z() const noexcept {return z;}

    void x_advance(){
        ++x;
        if(x!=x_end){
            y_to_begin(x);
            z_to_begin(y);
        }
    }
    void y_advance(){
        ++y;
        if(y!=y_end){
            z_to_begin(y);
        }
    }
    void z_reset(){z_to_begin(y);}

    Token_Kind_t left() const noexcept {return (x->first).left;}
    size_t rights_size() const noexcept {return (y->first).rights_size;}
    Token_Kind_t lookahead() const noexcept {return (y->first).lookahead;}
    size_t cursor() const noexcept {return (y->first).cursor;}
    Token_Kind_t cursored() const noexcept {return (x->first).cursored;}
    const std::basic_string<Token_Kind_t>& rights() const noexcept {return z->rights;}
    bool& processed() const noexcept {return z->processed;}
    bool is_complete() const noexcept {return cursored()==Token_Kind_t(-1);}
    const LR1_Item_Char_1<Token_Kind_t>& char1() const noexcept {return x->first;}
    const LR1_Item_Char_2<Token_Kind_t>& char2() const noexcept {return y->first;}
    #ifdef YUKI_PG_PRINT
    void print(FILE* fp){
    fmt::print(fp,"({} -> )",static_cast<unsigned long long>(left()));
    for(const Token_Kind_t i : z->rights)
        fmt::print(fp,"{},",static_cast<unsigned long long>(i));
    fmt::print(fp," | {},{}",static_cast<unsigned long long>(lookahead()),cursor());
    }
    #endif
}; // struct LR1_Item_Set<Token_Kind_t>::const_iterator
}

#ifdef YUKI_PG_PRINT
//template<std::unsigned_integral Token_Kind_t>
//struct fmt::formatter<typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::iterator,char> : yuki::simple_formatter<typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::iterator,char> {
//    template<typename OutputIt>
//    auto format(const typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::iterator& it,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
//        typename fmt::basic_format_context<OutputIt,char>::iterator o = ctx.out();
//        fmt::format_to(o,"({} -> )",static_cast<unsigned long long>(it.left()));
//        for(const Token_Kind_t i : it.rights())
//            fmt::format_to(o,"{},",static_cast<unsigned long long>(i));
//        return fmt::format_to(o," | {},{}",static_cast<unsigned long long>(it.lookahead()),it.cursor());
//    }
//};
//template<std::unsigned_integral Token_Kind_t>
//struct fmt::formatter<typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::const_iterator,char> : yuki::simple_formatter<typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::const_iterator,char> {
//    template<typename OutputIt>
//    auto format(const typename yuki::pg::lr1::LR1_Item_Set<Token_Kind_t>::const_iterator& it,fmt::basic_format_context<OutputIt,char>& ctx) -> typename fmt::basic_format_context<OutputIt,char>::iterator {
//        typename fmt::basic_format_context<OutputIt,char>::iterator o = ctx.out();
//        fmt::format_to(o,"({} -> )",static_cast<unsigned long long>(it.left()));
//        for(const Token_Kind_t i : it.rights())
//            fmt::format_to(o,"{},",static_cast<unsigned long long>(i));
//        return fmt::format_to(o," | {},{}",static_cast<unsigned long long>(it.lookahead()),it.cursor());
//    }
//};
#endif

namespace yuki::pg::lr1{
template<std::unsigned_integral Token_Kind_t>
void closure(LR1_Item_Set<Token_Kind_t>& items,const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable){
    const Token_Info& ti=ftable.get_token_info();
    size_t items_size = items.size();
    bool items_changed = true;
    while(items_changed){
        items_changed=false;
        typename LR1_Item_Set<Token_Kind_t>::iterator items_b = items.begin();
        typename LR1_Item_Set<Token_Kind_t>::iterator items_e = items.end();
        while(items_b!=items_e){
            item_loop_head:
            if(!items_b.processed()){
                items_b.processed()=true;
                if(!items_b.is_complete() && is_nterminal(ti,items_b.rights()[items_b.cursor()])){
                    typename Rule_Set<Token_Kind_t>::const_iterator b = begin_with_left(rules,items_b.rights()[items_b.cursor()]);
                    typename Rule_Set<Token_Kind_t>::const_iterator e = end_with_left(rules,items_b.rights()[items_b.cursor()]);
                    if(b!=rules.end()){
                        Token_Kind_t item_lookahead=items_b.lookahead();
                        std::basic_string_view<Token_Kind_t> item_rights(items_b.rights()); // The reallocation of `vector` should employ the move constructor, not copy then destruct, so the underlying data of `items_b.rights()` should remain intact.
                        size_t item_cursor = items_b.cursor();
                        for(;b!=e;++b){
                            typename First_Table<Token_Kind_t>::extended_const_iterator_ne e_i=ftable.begin_ne(item_rights.substr(item_cursor+1),item_lookahead);
                            for(;!e_i.is_end();++e_i){
                                items.insert({item_rights[item_cursor],b->rights,*e_i,0});
                            }
                        }
                        if(items.size()>items_size){
                            items_changed=true;
                            items_size=items.size();
                            items_b.z_reset(); // After the insertion, the vector iterator might be invalidated. Now I reset it.
                            goto item_loop_head;
                        }
                    }
                }
            }
            ++items_b;
        }
    }
}

template<std::unsigned_integral Token_Kind_t>
LR1_Item_Set<Token_Kind_t> go_to(const LR1_Item_Set<Token_Kind_t>& items,const std::type_identity_t<Token_Kind_t> kind,const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable){
    LR1_Item_Set<Token_Kind_t> moved{};
    typename LR1_Item_Set<Token_Kind_t>::const_iterator items_b = items.begin();
    for(;!items_b.is_end();++items_b){
        if(!items_b.is_complete() && items_b.rights()[items_b.cursor()]==kind)
            moved.insert({items_b.left(),items_b.rights(),items_b.lookahead(),items_b.cursor()+1});
    }
    closure(moved,rules,ftable);
    return moved;
}

template<std::unsigned_integral Token_Kind_t>
LR1_Item_Set<Token_Kind_t> generate_initial_items(const Rule_Set<Token_Kind_t>& rules,const First_Table<Token_Kind_t>& ftable){
    const Token_Info& ti=ftable.get_token_info();
    LR1_Item_Set<Token_Kind_t> items{};
    typename Rule_Set<Token_Kind_t>::const_iterator b = begin_with_left(rules,ti.goal);
    typename Rule_Set<Token_Kind_t>::const_iterator e = end_with_left(rules,ti.goal);
    if(b!=rules.end()){
        for(;b!=e;++b)
            items.insert({static_cast<Token_Kind_t>(ti.goal),b->rights,static_cast<Token_Kind_t>(ti.EOF_),0});
    }
    closure(items,rules,ftable);
    return items;
}

struct Prec_Table{
  private:
    prec_t* p;
    size_t index_offset;
  public:
    // Only terminals have precedence. Assigning precedence to non-terms seems to have little sense.
    explicit Prec_Table(const Token_Info& ti) noexcept :
        index_offset(ti.terminal_first)
    {
        p = new prec_t[ti.terminal_total()]{};
    }
    ~Prec_Table() noexcept {delete[] p;}
    Prec_Table(const Prec_Table&) = delete;
    Prec_Table(Prec_Table&& other) noexcept :
        p(other.p),
        index_offset(other.index_offset)
    {other.p = nullptr; other.index_offset = 0;}

    Prec_Table& operator=(const Prec_Table&) = delete;
    Prec_Table& operator=(Prec_Table&& other) noexcept {
        if(this!=&other){
            delete[] p;
            p = other.p;
            index_offset = other.index_offset;
            other.p = nullptr;
            other.index_offset = 0;
        }
        return *this;
    }

    prec_t& operator[](unsigned long long kind) {assert(kind>=index_offset); return p[kind-index_offset];}
    const prec_t& operator[](unsigned long long kind) const {assert(kind>=index_offset); return p[kind-index_offset];}
};

struct Assoc_Table{
  private:
    Assoc* p;
    size_t index_offset;
  public:
    Assoc zero;

    // Only terminals have precedence. Assigning precedence to non-terms seems to have little sense.
    explicit Assoc_Table(const Token_Info& ti,Assoc zero_p) noexcept :
        index_offset(ti.terminal_first),
        zero(zero_p)
    {
        p = new Assoc[ti.terminal_total()]{};
    }
    ~Assoc_Table() noexcept {delete[] p;}
    Assoc_Table(const Assoc_Table&) = delete;
    Assoc_Table(Assoc_Table&& other) noexcept :
        p(other.p),
        index_offset(other.index_offset),
        zero(other.zero)
    {other.p = nullptr; other.index_offset = 0; other.zero = Assoc::RIGHT;}

    Assoc_Table& operator=(const Assoc_Table&) = delete;
    Assoc_Table& operator=(Assoc_Table&& other) noexcept {
        if(this!=&other){
            delete[] p;
            p = other.p;
            index_offset = other.index_offset;
            zero = other.zero;
            other.p = nullptr;
            other.index_offset = 0;
            other.zero = Assoc::RIGHT;
        }
        return *this;
    }

    Assoc& operator[](unsigned long long kind) {assert(kind>=index_offset); return p[kind-index_offset];}
    const Assoc& operator[](unsigned long long kind) const {assert(kind>=index_offset); return p[kind-index_offset];}
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
        if(shift!=0){
            if(reduce.rule_num!=0)
                return State::SR;
            else
                return State::S;
        }else{
            if(reduce.rule_num!=0)
                return State::R;
            else
                return State::EMPTY;
        }
    }
    constexpr bool empty() const noexcept {return shift==0 && reduce.rule_num==0;}
};

void resolve_rr_conflict(Action_Candidates& ac1,const size_t rule_num_2,const prec_t prec_sr_2,const prec_t prec_rr_2){
    // First select the rule with a higher rr-precedence; if same, then select the rule that has the smaller number, i.e. the rule that appears earlier in the grammar specs.
    // Note that `prec_sr` does not participate in the resolution process. It is recored for later SR-conflict resolution.
    if( (ac1.reduce.prec_rr<prec_rr_2) || ((ac1.reduce.prec_rr==prec_rr_2) && (ac1.reduce.rule_num>rule_num_2)) )
        ac1.reduce={rule_num_2,prec_sr_2,prec_rr_2};
}

enum struct Action_Kind {SHIFT,REDUCE}; // There is a enum with the same name in the target-parser header "lr1.hpp". That should be fine, however, since it makes no sense to include the target-parser header in this so to speak "meta-parser" file.

Action_Kind resolve_sr_conflict(const Prec_Table& prec_table,const Assoc_Table& assoc_table,const Action_Candidates& ac,const unsigned long long lookahead){
    if(ac.reduce.prec_sr<prec_table[lookahead]){
        return Action_Kind::SHIFT;
    }else if(ac.reduce.prec_sr>prec_table[lookahead]){
        return Action_Kind::REDUCE;
    }else{
        switch(ac.reduce.prec_sr==0 ? assoc_table.zero : assoc_table[lookahead]){
            case Assoc::RIGHT : return Action_Kind::SHIFT;
            case Assoc::LEFT : return Action_Kind::REDUCE;
            default : return Action_Kind::SHIFT;
        }
    }
}
}