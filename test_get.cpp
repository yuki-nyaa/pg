#include<iostream>
#include"token.cpp"
using Foo=Token::Token_Concrete<Token_Kind::LB,const char*,double>;

const Foo foofunc_const(){
    Foo t = {"Foo", 3.14};
    return t;
}
Foo foofunc(){
    Foo t = {"Foo", 3.14};
    return t;
}
int main()
{
    //Foo t = {"Foo", 3.14};
    //const Foo t_const = {"Foo", 3.14};

    //std::cout << t.get<const char*>() <<"\n"
    //          << t.get<double>() << "\n";
    //typedef decltype(t.get<const char*>()) bar1;
    //typedef std::remove_reference_t<bar1> bar1_noref;
    //typedef decltype(t.get<double>()) bar2;
    //typedef std::remove_reference_t<bar2> bar2_noref;    
    //std::cout<<std::is_lvalue_reference_v<bar1><<std::is_rvalue_reference_v<bar1><<std::is_const_v<bar1_noref><<std::endl;
    //std::cout<<std::is_lvalue_reference_v<bar2><<std::is_rvalue_reference_v<bar2><<std::is_const_v<bar2_noref><<std::endl;
    //std::cout<<std::endl;

    //std::cout << t_const.get<const char*>() <<"\n"
    //          << t_const.get<double>() << "\n";
    //typedef decltype(t_const.get<const char*>()) bbar1;
    //typedef std::remove_reference_t<bbar1> bbar1_noref;
    //typedef decltype(t_const.get<double>()) bbar2;
    //typedef std::remove_reference_t<bbar2> bbar2_noref;    
    //std::cout<<std::is_lvalue_reference_v<bbar1><<std::is_rvalue_reference_v<bbar1><<std::is_const_v<bbar1_noref><<std::endl;
    //std::cout<<std::is_lvalue_reference_v<bbar2><<std::is_rvalue_reference_v<bbar2><<std::is_const_v<bbar2_noref><<std::endl;
    //std::cout<<std::endl;   

    //std::cout << foofunc().get<const char*>() <<"\n"
    //          << foofunc().get<double>() << "\n";
    //typedef decltype(foofunc().get<const char*>()) bbbar1;
    //typedef std::remove_reference_t<bbbar1> bbbar1_noref;
    //typedef decltype(foofunc().get<double>()) bbbar2;
    //typedef std::remove_reference_t<bbbar2> bbbar2_noref;    
    //std::cout<<std::is_lvalue_reference_v<bbbar1><<std::is_rvalue_reference_v<bbbar1><<std::is_const_v<bbbar1_noref><<std::endl;
    //std::cout<<std::is_lvalue_reference_v<bbbar2><<std::is_rvalue_reference_v<bbbar2><<std::is_const_v<bbbar2_noref><<std::endl;
    //std::cout<<std::endl;      

    //std::cout << foofunc_const().get<const char*>() <<"\n"
    //          << foofunc_const().get<double>() << "\n";
    //typedef decltype(foofunc_const().get<const char*>()) bbbbar1;
    //typedef std::remove_reference_t<bbbbar1> bbbbar1_noref;
    //typedef decltype(foofunc_const().get<double>()) bbbbar2;
    //typedef std::remove_reference_t<bbbbar2> bbbbar2_noref;    
    //std::cout<<std::is_lvalue_reference_v<bbbbar1><<std::is_rvalue_reference_v<bbbbar1><<std::is_const_v<bbbbar1_noref><<std::endl;
    //std::cout<<std::is_lvalue_reference_v<bbbbar2><<std::is_rvalue_reference_v<bbbbar2><<std::is_const_v<bbbbar2_noref><<std::endl;
    //std::cout<<std::endl;

    Foo t = {"Foo", 3.14};
    const Foo t_const = {"Foo", 3.14};

    std::cout << t.get<0>() <<"\n"
              << t.get<1>() << "\n";
    typedef decltype(t.get<0>()) bar1;
    typedef std::remove_reference_t<bar1> bar1_noref;
    typedef decltype(t.get<1>()) bar2;
    typedef std::remove_reference_t<bar2> bar2_noref;    
    std::cout<<std::is_lvalue_reference_v<bar1><<std::is_rvalue_reference_v<bar1><<std::is_const_v<bar1_noref><<std::endl;
    std::cout<<std::is_lvalue_reference_v<bar2><<std::is_rvalue_reference_v<bar2><<std::is_const_v<bar2_noref><<std::endl;
    std::cout<<std::endl;

    std::cout << t_const.get<0>() <<"\n"
              << t_const.get<1>() << "\n";
    typedef decltype(t_const.get<0>()) bbar1;
    typedef std::remove_reference_t<bbar1> bbar1_noref;
    typedef decltype(t_const.get<1>()) bbar2;
    typedef std::remove_reference_t<bbar2> bbar2_noref;    
    std::cout<<std::is_lvalue_reference_v<bbar1><<std::is_rvalue_reference_v<bbar1><<std::is_const_v<bbar1_noref><<std::endl;
    std::cout<<std::is_lvalue_reference_v<bbar2><<std::is_rvalue_reference_v<bbar2><<std::is_const_v<bbar2_noref><<std::endl;
    std::cout<<std::endl;   

    std::cout << foofunc().get<0>() <<"\n"
              << foofunc().get<1>() << "\n";
    typedef decltype(foofunc().get<0>()) bbbar1;
    typedef std::remove_reference_t<bbbar1> bbbar1_noref;
    typedef decltype(foofunc().get<1>()) bbbar2;
    typedef std::remove_reference_t<bbbar2> bbbar2_noref;    
    std::cout<<std::is_lvalue_reference_v<bbbar1><<std::is_rvalue_reference_v<bbbar1><<std::is_const_v<bbbar1_noref><<std::endl;
    std::cout<<std::is_lvalue_reference_v<bbbar2><<std::is_rvalue_reference_v<bbbar2><<std::is_const_v<bbbar2_noref><<std::endl;
    std::cout<<std::endl;      

    std::cout << foofunc_const().get<0>() <<"\n"
              << foofunc_const().get<1>() << "\n";
    typedef decltype(foofunc_const().get<0>()) bbbbar1;
    typedef std::remove_reference_t<bbbbar1> bbbbar1_noref;
    typedef decltype(foofunc_const().get<1>()) bbbbar2;
    typedef std::remove_reference_t<bbbbar2> bbbbar2_noref;    
    std::cout<<std::is_lvalue_reference_v<bbbbar1><<std::is_rvalue_reference_v<bbbbar1><<std::is_const_v<bbbbar1_noref><<std::endl;
    std::cout<<std::is_lvalue_reference_v<bbbbar2><<std::is_rvalue_reference_v<bbbbar2><<std::is_const_v<bbbbar2_noref><<std::endl;
    std::cout<<std::endl;            
}