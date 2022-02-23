#include"lr1_writer.h"

template<std::unsigned_integral Token_Kind_t>
void print_item(typename yuki::pg::LR1_Writer<Token_Kind_t>::LR1_Item_Set::const_iterator i){
    printf("(%u,(",i.left());
    for(Token_Kind_t r : i.rights())
        printf("%u,",r);
    printf("),%u,%zu)",i.lookahead(),i.cursor());
}

int main(){
    // Tokens: {(=0,),EOF_  List=3, Pair, epsilon_=5}
    // Tokens: {List=0,Pair, (=2,),EOF_,=4, epsilon_=5}
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
    yuki::pg::Rule_Set<unsigned short> rs;
    rs.emplace(-1,{0},0);

    rs.emplace(0,{0,1},1);
    rs.emplace(0,{1},2);

    rs.emplace(1,{2,1,3},3);
    rs.emplace(1,{2,3},4);

    yuki::pg::First_Table<unsigned short> ft(2,3,rs);

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set items;
    items.emplace(3,{3,4},2,0);
    items.emplace(3,{3,4},0,0);
    items.emplace(3,{4},2,0);
    items.emplace(3,{4},0,0);
    items.emplace(4,{0,4,1},2,0);
    items.emplace(4,{0,4,1},0,0);
    items.emplace(4,{0,1},2,0);
    items.emplace(4,{0,1},0,0);
    /*
    (4,(0,1,),0,0)
    (4,(0,1,),2,0)
    (4,(0,4,1,),0,0)
    (4,(0,4,1,),2,0)
    (3,(3,4,),0,0)
    (3,(3,4,),2,0)
    (3,(4,),0,0)
    (3,(4,),2,0)
    */
    printf("items\n");
    for(auto i=items.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 8 3
    yuki::print_space(stdout,items.size(),items.block_count());
    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set cc0 = yuki::pg::LR1_Writer<unsigned short>::generate_initial_items(rs,ft);
    /*
    (4,(0,1,),0,0)
    (4,(0,1,),2,0)
    (4,(0,4,1,),0,0)
    (4,(0,4,1,),2,0)
    (3,(3,4,),0,0)
    (3,(3,4,),2,0)
    (3,(4,),0,0)
    (3,(4,),2,0)
    */
    printf("cc0\n");
    for(auto i=cc0.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 9 4
    yuki::print_space(stdout,cc0.size(),cc0.block_count());

    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set cc3 = yuki::pg::LR1_Writer<unsigned short>::go_to(cc0,2,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),0,1)
    (4,(0,1,),2,1)
    (4,(0,4,1,),0,1)
    (4,(0,4,1,),2,1)
    */
    printf("cc3\n");
    for(auto i=cc3.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 6 3
    yuki::print_space(stdout,cc3.size(),cc3.block_count());

    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set cc1 = yuki::pg::LR1_Writer<unsigned short>::go_to(cc0,0,rs,ft);
    /*
    (4,(0,1,),0,0)
    (4,(0,1,),2,0)
    (4,(0,4,1,),0,0)
    (4,(0,4,1,),2,0)
    (3,(3,4,),0,1)
    (3,(3,4,),2,1)
    */
    printf("cc1\n");
    for(auto i=cc1.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 7 3
    yuki::print_space(stdout,cc1.size(),cc1.block_count());

    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set cc6 = yuki::pg::LR1_Writer<unsigned short>::go_to(cc3,2,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),1,1)
    (4,(0,4,1,),1,1)
    */
    printf("cc6\n");
    for(auto i=cc6.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 4 3
    yuki::print_space(stdout,cc6.size(),cc6.block_count());

    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set cc6_2 = yuki::pg::LR1_Writer<unsigned short>::go_to(cc6,2,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),1,1)
    (4,(0,4,1,),1,1)
    */
    printf("cc6_2\n");
    for(auto i=cc6_2.begin();!i.is_end();++i){
        print_item<unsigned short>(i);
        printf("\n");
    }
    // 4 3
    yuki::print_space(stdout,cc6_2.size(),cc6_2.block_count());

    printf("\n");

    yuki::pg::LR1_Writer<unsigned short>::LR1_Item_Set_Set ccs;
    ccs.insert(std::move(cc6));
    assert(ccs.insert(std::move(cc6_2)).first==false);
    ccs.insert(std::move(cc0));
    assert(ccs.insert(std::move(cc1)).first==true);
    assert(ccs.insert(std::move(cc3)).first==true);
}