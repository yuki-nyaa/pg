#include"../include/yuki/ParserGen/token2.hpp"
#include<cstdio>
//template<size_t kind>
//using token_t = yuki::pg::Token<kind,int,double,const char*,const int&>;
//static_assert(std::is_aggregate_v<token_t<10>>);
int main(){
    //using namespace yuki::pg;
    //token_t<1> token1={10,3.14,"foo",20};
    //token_t<10> token10={10,3.14,"foo",20};
    //printf("%d %f %s %d\n",token10.get<0>(),token10.get<1>(),token10.get<2>(),token10.get<3>());
    //printf("%d %f %s %d\n",token10.get<int>(),token10.get<double>(),token10.get<const char*>(),token10.get<const int&>());
    //static_assert(std::is_same_v<token_t<10>::member_type<0>,int>);
    //Token_Base* pb_token1 = static_cast<Token_Base*>(&token1);
    //Token_Base* pb_token10 = static_cast<Token_Base*>(&token10);
    //printf("%d %d",pb_token1->kind,pb_token10->kind);
    //token_t<1>* p_token1=static_cast<token_t<1>*>(pb_token1);
    //token_t<10>* p_token10=static_cast<token_t<10>*>(pb_token10);
}