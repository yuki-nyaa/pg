#include<yuki/ParserGen/core.hpp>
struct Foo{
    Foo()=delete;
};
template<unsigned short k,typename T0,typename T1>
using Token =  yuki::pg::Token_Concrete_General<unsigned short,k,T0,T1>;
int main(){
    Token<5,Foo,int> token1{};
}
