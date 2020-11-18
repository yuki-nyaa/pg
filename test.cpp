#include<iostream>
#include<vector>
#include<string>
#include<type_traits>
#include<set>
#include<algorithm>
#include<yuki/utils.hpp>
struct Rule{
    unsigned int left;
    std::basic_string<unsigned int> rights;

    friend bool operator==(const Rule&,const Rule&) = default;
};
template<>
struct yuki::rough_less<Rule>{
    bool operator()(const Rule& lhs,const Rule& rhs) const {
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
int main(){
    std::multiset<Rule,yuki::rough_less<Rule>> s;
    Rule r1 = {10,{1,2,3,4}};
    Rule r2 = {10,{2,3,4,5}};
    s.insert(r1);
    s.insert(r2);
    auto p = s.find(r2);
    std::cout<<p->rights[0];
}