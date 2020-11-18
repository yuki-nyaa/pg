#pragma once
#include"common.hpp"
#include<concepts>
#include<yuki/utils.hpp>
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
        size_t cursor;

        friend bool operator==(const LR1_Item&, const LR1_Item&) = default;
    };

    template<std::unsigned_integral Token_Kind_t>
    struct yuki::rough_less<LR1_Item<Token_Kind_t>>{
        bool operator()(const LR1_Item<Token_Kind_t>& lhs, const LR1_Item<Token_Kind_t>& rhs) const {
            if(lhs.left<rhs.left)
                return true;
            else if(rhs.left<lhs.left)
                return false;
            else if(lhs.rights.size()<rhs.rights.size())
                return true;
            else if(rhs.rights.size()<lhs.rights.size())
                return false;
            else if(lhs.lookahead<rhs.lookahead)
                return true;
            else if(rhs.lookahead<lhs.lookahead)
                return false;
            else if(lhs.cursor<rhs.cursor)
                return true;
            else if(rhs.cursor<lhs.cursor)
                return false;
            return false;
        }
    };

    template<std::unsigned_integral Token_Kind_t>
    struct yuki::rough_greater<LR1_Item<Token_Kind_t>>{
        bool operator()(const LR1_Item<Token_Kind_t>& lhs, const LR1_Item<Token_Kind_t>& rhs) const {
            if(lhs.left>rhs.left)
                return true;
            else if(rhs.left>lhs.left)
                return false;
            else if(lhs.rights.size()>rhs.rights.size())
                return true;
            else if(rhs.rights.size()>lhs.rights.size())
                return false;
            else if(lhs.lookahead>rhs.lookahead)
                return true;
            else if(rhs.lookahead>lhs.lookahead)
                return false;
            else if(lhs.cursor>rhs.cursor)
                return true;
            else if(rhs.cursor>lhs.cursor)
                return false;
            return false;
        }
    };

    template<std::unsigned_integral Token_Kind_t>
    using LR1_Item_Set = std::multiset<LR1_Item<Token_Kind_t>,rough_less<Token_Kind_t>>;

    template<std::unsigned_integral Token_Kind_t>
    LR1_Item_Set<Token_Kind_t>& lr1_closure(const Token_Info& ti,LR1_Item_Set<Token_Kind_t>& item_set){
        bool changed = true;
        while(changed){
            for(const LR1_Item& item : item_set){
                if(item.rights.size()>=item.cursor+1 && is_nterminal(ti,(item.rights)[cursor+1])){
                    for(const Rule& rule : rule_set){
                        if(starts with (item.rights)[cursor+1]){
                            for(const Token_Kind_t first_item : first())
                        }
                    }
                }
            }
        }
    }
}