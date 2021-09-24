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
    yuki::pg::Token_Info t_info1 = {true,0,4,5,13};
    yuki::pg::Rule_Set<unsigned short> rs1;
    rs1.insert_equal({0,{2,1},1});

    rs1.insert_equal({1,{7,2,1},2});
    rs1.insert_equal({1,{8,2,1},3});
    rs1.insert_equal({1,{14},4});

    rs1.insert_equal({2,{4,3},5});

    rs1.insert_equal({3,{9,8,3},6});
    rs1.insert_equal({3,{10,4,3},7});
    rs1.insert_equal({3,{14},8});

    rs1.insert_equal({4,{11,0,12},9});
    rs1.insert_equal({4,{5},10});
    rs1.insert_equal({4,{6},11});

    yuki::pg::First_Table<unsigned short> ft1(t_info1,rs1);
    for(const auto& fs : ft1){
        std::cout<<fs<<'\n';
    }
    std::cout<<std::endl;

    // 2 3 0 1 6
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei = ft1.begin_ne({10},9);
    for(;!ei.is_end();++ei){
        std::cout<<*ei<<' ';
    }
    std::cout<<std::endl;

    // 4 5 2 3
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei2 = ft1.begin_ne({12},10);
    for(;!ei2.is_end();++ei2){
        std::cout<<*ei2<<' ';
    }
    std::cout<<std::endl;

    //
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei3 = ft1.begin_ne({14,14},14);
    for(;!ei3.is_end();++ei3){
        std::cout<<*ei3<<' ';
    }
    std::cout<<std::endl;

    // 0 1 6
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei4 = ft1.begin_ne({9,2},3);
    for(;!ei4.is_end();++ei4){
        std::cout<<*ei4<<' ';
    }
    std::cout<<std::endl;


    std::cout<<'\n';



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
    yuki::pg::Token_Info t_info2 = {true,0,1,2,4};
    yuki::pg::Rule_Set<unsigned short> rs2;
    rs2.insert_equal({0,{0,1},1});
    rs2.insert_equal({0,{1},2});

    rs2.insert_equal({1,{2,1,3},3});
    rs2.insert_equal({1,{2,3},4});

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

    // 0
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei5(&ft2,{3,4},2);
    for(;!ei5.is_end();++ei5){
        std::cout<<*ei5<<' ';
    }
    std::cout<<std::endl;

    // 1
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei6(&ft2,{1},0);
    for(;!ei6.is_end();++ei6){
        std::cout<<*ei6<<' ';
    }
    std::cout<<std::endl;

    //
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei7(&ft2,{5},5);
    for(;!ei7.is_end();++ei7){
        std::cout<<*ei7<<' ';
    }
    std::cout<<std::endl;

    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei8(&ft2,{4},2);
    for(;!ei8.is_end();++ei8){
        std::cout<<*ei8<<' ';
    }
    std::cout<<std::endl;
}