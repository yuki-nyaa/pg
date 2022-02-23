#include<cstdio>
#include"common.hpp"
int main(){
    // Tokens: {Expr=0,Expr',Term,Term',Factor, num=5,name,+,-,*,/,(,),EOF_=13, epsilon_=14}
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
        (5,6,11,)
        (7,8,14,)
        (5,6,11,)
        (9,10,14,)
        (5,6,11,)
        (5,)
        (6,)
        (7,)
        (8,)
        (9,)
        (10,)
        (11,)
        (12,)
        (13,)
        (14,)
    */
    yuki::pg::Rule_Set<unsigned short> rs1;
    rs1.emplace(0,{2,1},1);

    rs1.emplace(1,{7,2,1},2);
    rs1.emplace(1,{8,2,1},3);
    rs1.emplace(1,{14},4);

    rs1.emplace(2,{4,3},5);

    rs1.emplace(3,{9,8,3},6);
    rs1.emplace(3,{10,4,3},7);
    rs1.emplace(3,{14},8);

    rs1.emplace(4,{11,0,12},9);
    rs1.emplace(4,{5},10);
    rs1.emplace(4,{6},11);

    yuki::pg::First_Table<unsigned short> ft1(5,9,rs1);
    for(size_t i=0;i<15;++i){
        printf("(");
        for(auto n : ft1[i])
            printf("%u,",n);
        printf(")\n");
    }

    printf("\n");

    // 7 8 5 6 11
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei = ft1.begin_ne({1},0);
    for(;!ei.is_end();++ei)
        printf("%u ",*ei);
    printf("\n");

    // 9 10 7 8
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei2 = ft1.begin_ne({3},1);
    for(;!ei2.is_end();++ei2)
        printf("%u ",*ei2);
    printf("\n");

    //
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei3 = ft1.begin_ne({14,14},14);
    for(;!ei3.is_end();++ei3)
        printf("%u ",*ei3);
    printf("\n");

    // 5 6 11
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei4 = ft1.begin_ne({0,7},8);
    for(;!ei4.is_end();++ei4)
        printf("%u ",*ei4);
    printf("\n");


    printf("\n");



    // Tokens: {(=0,),EOF_  List=3, Pair, epsilon_=5}
    // Tokens: {List=0, Pair, (=2,),EOF_=4  epsilon_=5}
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
    yuki::pg::Rule_Set<unsigned short> rs2;
    rs2.emplace(0,{0,1},1);
    rs2.emplace(0,{1},2);

    rs2.emplace(1,{2,1,3},3);
    rs2.emplace(1,{2,3},4);

    yuki::pg::First_Table<unsigned short> ft2(2,3,rs2);
    /*
    (2,)
    (2,)
    (2,)
    (3,)
    (4,)
    (5,)
    */
    for(size_t i=0;i<6;++i){
        printf("(");
        for(auto n : ft2[i])
            printf("%u,",n);
        printf(")\n");
    }

    printf("\n");

    // 2
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei5(&ft2,{0,1},4);
    for(;!ei5.is_end();++ei5)
        printf("%u ",*ei5);
    printf("\n");

    // 3
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei6(&ft2,{3},2);
    for(;!ei6.is_end();++ei6)
        printf("%u ",*ei6);
    printf("\n");

    //
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei7(&ft2,{5},5);
    for(;!ei7.is_end();++ei7)
        printf("%u ",*ei7);
    printf("\n");

    // 2
    yuki::pg::First_Table<unsigned short>::extended_const_iterator_ne ei8(&ft2,{1},4);
    for(;!ei8.is_end();++ei8)
        printf("%u ",*ei8);
    printf("\n");
}