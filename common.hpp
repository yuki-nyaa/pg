#pragma once
#include<concepts>
#include<string>
#include<yuki/utils.hpp>
namespace yuki::pg{
    struct Token_Info{
        bool has_T;
        size_t T_start;
        size_t T_end;
        bool has_NT;
        size_t NT_start;
        size_t NT_end;
        constexpr size_t total() noexcept {
            size_t sum=0;
            if(has_T)
                sum+=(T_end-T_start+1);
            if(has_NT)
                sum+=(NT_end-NT_start+1);
            return sum;
        }
    }

    constexpr bool is_terminal(const Token_Info& ti,size_t kind) noexcept {return ti.has_T && kind>=ti.T_start && kind<=ti.T_end;}
    constexpr bool is_nterminal(const Token_Info& ti,size_t kind) noexcept {return ti.has_NT && kind>=ti.NT_start && kind<=ti.NT_end;}

    template<std::unsigned_integral Token_Kind_t>
    struct Rule{
        Token_Kind_t left;
        std::basic_string<Token_Kind_t> rights;

        friend bool operator==(const Rule&,const Rule&) = default;
    };

    template<std::unsigned_integral Token_Kind_t>
    struct yuki::rough_less<Rule<Token_Kind_t>>{
        bool operator()(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs) const {
            if(lhs.left<rhs.left)
                return true;
            else if(rhs.left<lhs.left)
                return false;
            else if(lhs.rights.size()<rhs.rights.size())
                return true;
            else if(rhs.rights.size()<lhs.rights.size())
                return false;
            return false;
        }
    };

    template<std::unsigned_integral Token_Kind_t>
    struct yuki::rough_greater<Rule<Token_Kind_t>>{
        bool operator()(const Rule<Token_Kind_t>& lhs,const Rule<Token_Kind_t>& rhs) const {
            if(lhs.left>rhs.left)
                return true;
            else if(rhs.left>lhs.left)
                return false;
            else if(lhs.rights.size()>rhs.rights.size())
                return true;
            else if(rhs.rights.size()>lhs.rights.size())
                return false;
            return false;
        }
    };

    template<std::unsigned_integral Token_Kind_t>
    using Rule_Set = std::multiset<Rule<Token_Kind_t>,yuki::rough_less<Rule<Token_Kind_t>>>;


    template<std::unsigned_integral Token_Kind_t>
    using First_Set = std::set<Token_Kind_t>;

    typedef std::unique_ptr<First_Set[]> First_Table;

    First_Table allocate_first_table(size_t size){return std::unique_ptr<First_Set[]>(new First_Set[size]);}

    compute_first(const)

}