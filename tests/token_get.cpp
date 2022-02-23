#include<yuki/tmp.hpp>
#include"yuki/pg/core.hpp"

using Foo=yuki::pg::Token<unsigned,0,const char*,double,int,int>;

const Foo foofunc_const(){return {"Foo", 3.14};}
Foo foofunc(){return {"Foo", 3.14};}
int main(){
    Foo t = {"Foo", 3.14};
    const Foo t_const = {"Foo", 3.14};

    yuki::print_space(stdout,t.get<const char*>(),t.get<double>());
    typedef decltype(t.get<const char*>()) bar1;
    typedef std::remove_reference_t<bar1> bar1_noref;
    typedef decltype(t.get<double>()) bar2;
    typedef std::remove_reference_t<bar2> bar2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bar1>,std::is_rvalue_reference_v<bar1>,std::is_const_v<bar1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bar2>,std::is_rvalue_reference_v<bar2>,std::is_const_v<bar2_noref>);

    yuki::print_space(stdout,t_const.get<const char*>(),t_const.get<double>());
    typedef decltype(t_const.get<const char*>()) bbar1;
    typedef std::remove_reference_t<bbar1> bbar1_noref;
    typedef decltype(t_const.get<double>()) bbar2;
    typedef std::remove_reference_t<bbar2> bbar2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbar1>,std::is_rvalue_reference_v<bbar1>,std::is_const_v<bbar1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbar2>,std::is_rvalue_reference_v<bbar2>,std::is_const_v<bbar2_noref>);

    yuki::print_space(stdout,foofunc().get<const char*>(),foofunc().get<double>());
    typedef decltype(foofunc().get<const char*>()) bbbar1;
    typedef std::remove_reference_t<bbbar1> bbbar1_noref;
    typedef decltype(foofunc().get<double>()) bbbar2;
    typedef std::remove_reference_t<bbbar2> bbbar2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbar1>,std::is_rvalue_reference_v<bbbar1>,std::is_const_v<bbbar1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbar2>,std::is_rvalue_reference_v<bbbar2>,std::is_const_v<bbbar2_noref>);

    yuki::print_space(stdout,foofunc_const().get<const char*>(),foofunc_const().get<double>());
    typedef decltype(foofunc_const().get<const char*>()) bbbbar1;
    typedef std::remove_reference_t<bbbbar1> bbbbar1_noref;
    typedef decltype(foofunc_const().get<double>()) bbbbar2;
    typedef std::remove_reference_t<bbbbar2> bbbbar2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbbar1>,std::is_rvalue_reference_v<bbbbar1>,std::is_const_v<bbbbar1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbbar2>,std::is_rvalue_reference_v<bbbbar1>,std::is_const_v<bbbbar2_noref>);


    printf("\n\n");


    yuki::print_space(stdout,t.get<0>(),t.get<1>());
    typedef decltype(t.get<0>()) baz1;
    typedef std::remove_reference_t<baz1> baz1_noref;
    typedef decltype(t.get<1>()) baz2;
    typedef std::remove_reference_t<baz2> baz2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<baz1>,std::is_rvalue_reference_v<baz1>,std::is_const_v<baz1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<baz2>,std::is_rvalue_reference_v<baz2>,std::is_const_v<baz2_noref>);

    yuki::print_space(stdout,t_const.get<0>(),t_const.get<1>());
    typedef decltype(t_const.get<0>()) bbaz1;
    typedef std::remove_reference_t<bbaz1> bbaz1_noref;
    typedef decltype(t_const.get<1>()) bbaz2;
    typedef std::remove_reference_t<bbaz2> bbaz2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbaz1>,std::is_rvalue_reference_v<bbaz1>,std::is_const_v<bbaz1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbaz2>,std::is_rvalue_reference_v<bbaz2>,std::is_const_v<bbaz2_noref>);

    yuki::print_space(stdout,foofunc().get<0>(),foofunc().get<1>());
    typedef decltype(foofunc().get<0>()) bbbaz1;
    typedef std::remove_reference_t<bbbaz1> bbbaz1_noref;
    typedef decltype(foofunc().get<1>()) bbbaz2;
    typedef std::remove_reference_t<bbbaz2> bbbaz2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbaz1>,std::is_rvalue_reference_v<bbbaz1>,std::is_const_v<bbbaz1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbaz2>,std::is_rvalue_reference_v<bbbaz2>,std::is_const_v<bbbaz2_noref>);

    yuki::print_space(stdout,foofunc_const().get<0>(),foofunc_const().get<1>());
    typedef decltype(foofunc_const().get<0>()) bbbbaz1;
    typedef std::remove_reference_t<bbbbaz1> bbbbaz1_noref;
    typedef decltype(foofunc_const().get<1>()) bbbbaz2;
    typedef std::remove_reference_t<bbbbaz2> bbbbaz2_noref;
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbbaz1>,std::is_rvalue_reference_v<bbbbaz1>,std::is_const_v<bbbbaz1_noref>);
    yuki::print_space(stdout,std::is_lvalue_reference_v<bbbbaz2>,std::is_rvalue_reference_v<bbbbaz2>,std::is_const_v<bbbbaz2_noref>);
}