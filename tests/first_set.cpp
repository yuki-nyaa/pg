#define YUKI_PG_PRINT
#include"common.hpp"
int main(){
    // Tokens: {num=0,name,+,-,*,/,(,),EOF_, Expr=9,Expr',Term,Term',Factor, epsilon_=14}
    /* Rules:
        Goal:
            Expr ;
        Expr:
            Term Expr' ;
        Expr':
            + Term Expr' |
            - Term Expr' |
            epsilon_ ;
        Term:
            Factor Term' ;
        Term':
            * Factor Term' |
            / Factor Term' |
            epsilon_ ;
        Factor:
            ( Expr ) |
            num |
            name ;
    */
    /* Expected output:
        (0,)
        (1,)
        (2,)
        (3,)
        (4,)
        (5,)
        (6,)
        (7,)
        (8,)
        (0,1,6,)
        (2,3,14,)
        (0,1,6,)
        (4,5,14,)
        (0,1,6,)
        (14,)
    */
    yuki::pg::Token_Info t_info1 = {8,true,13};
    yuki::pg::Rule_Set<unsigned short> rs1;
    rs1.insert_equal({9,{11,10}});

    rs1.insert_equal({10,{2,11,10}});
    rs1.insert_equal({10,{3,11,10}});
    rs1.insert_equal({10,{14}});

    rs1.insert_equal({11,{13,12}});

    rs1.insert_equal({12,{4,3,12}});
    rs1.insert_equal({12,{5,13,12}});
    rs1.insert_equal({12,{14}});

    rs1.insert_equal({13,{6,9,7}});
    rs1.insert_equal({13,{0}});
    rs1.insert_equal({13,{1}});

    yuki::pg::First_Table<unsigned short> ft1(t_info1,rs1);
    for(const auto& fs : ft1){
        std::cout<<fs<<'\n';
    }
    std::cout<<std::endl;


    const std::basic_string<unsigned short> str1={10,9};
    const std::basic_string<unsigned short> str2={10,9};
    const std::basic_string<unsigned short> str3={12,10};
    // 1 0 1
    yuki::print_space(std::cout,ft1.extended_contains(1,{10,9}),ft1.extended_contains(14,{10,9}),ft1.extended_contains(14,{12,10}));
    // 1 1 0
    yuki::print_space(std::cout,ft1.extended_contains(1,str1),ft1.extended_contains(1,{10,9},10),ft1.extended_contains(14,str1,9));


    std::cout<<'\n';


    // 2 3 0 1 6 6
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei = ft1.begin({10,9});
    do{
        std::cout<<*ei<<' ';
    }while(ei.increment_if_not_last());
    std::cout<<std::endl;

    // 4 5 2 3 14
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei2 = ft1.begin({12,10});
    do{
        std::cout<<*ei2<<' ';
    }while(ei2.increment_if_not_last());
    std::cout<<std::endl;

    // 14
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei3 = ft1.begin({14,14,14});
    do{
        std::cout<<*ei3<<' ';
    }while(ei3.increment_if_not_last());
    std::cout<<std::endl;

    // 0 1 6 6
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei4 = ft1.begin({9,2,3});
    do{
        std::cout<<*ei4<<' ';
    }while(ei4.increment_if_not_last());
    std::cout<<std::endl;


    std::cout<<'\n';


    // 0 1 2 3 6
    yuki::pg::First_Table<unsigned short>::set_type s = ft1.extended_get({10,9});
    for(const unsigned short e : s){
        std::cout<<e<<' ';
    }
    std::cout<<std::endl;

    // 2 3 4 5 14
    yuki::pg::First_Table<unsigned short>::set_type s2 = ft1.extended_get({12,10});
    for(const unsigned short e : s2){
        std::cout<<e<<' ';
    }std::cout<<std::endl;


    std::cout<<'\n';




    std::cout<<"-------------------------------------------------------"<<std::endl;




    // Tokens: {(=0,),EOF_  List=3, Pair, epsilon_=5}
    /* Rules:
        Goal:
            List ;
        list:
            List Pair
            Pair ;
        Pair:
            ( Pair )
            ( )
    */
    yuki::pg::Token_Info t_info2 = {2,true,4};
    yuki::pg::Rule_Set<unsigned short> rs2;
    rs2.insert_equal({3,{3,4}});
    rs2.insert_equal({3,{4}});

    rs2.insert_equal({4,{0,4,1}});
    rs2.insert_equal({4,{0,1}});

    yuki::pg::First_Table<unsigned short> ft2(t_info2,rs2);
    /*
    (0,)
    (1,)
    (2,)
    (0,)
    (0,)
    (5,)
    */
    for(const auto& fs : ft2){
        std::cout<<fs<<'\n';
    }

    std::cout<<std::endl;

    // 0 0
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei5(&ft2,{3,4},2);
    do{
        std::cout<<*ei5<<' ';
    }while(ei5.increment_if_not_last());
    std::cout<<std::endl;

    // 1 1
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei6(&ft2,{1},0);
    do{
        std::cout<<*ei6<<' ';
    }while(ei6.increment_if_not_last());
    std::cout<<std::endl;

    auto s3 = ft2.extended_get({1},0);
    // 1
    for(const auto& item : s3){
        std::cout<<item<<' ';
    }
    std::cout<<std::endl;

    // 0 1
    yuki::print_space(std::cout,ft2.extended_contains(0,{1},0),ft2.extended_contains(1,{1},0));

    // 5
    yuki::pg::First_Table<unsigned short>::extended_const_iterator ei7(&ft2,{5},5);
    do{
        std::cout<<*ei7<<' ';
    }while(ei7.increment_if_not_last());
    std::cout<<std::endl;
}