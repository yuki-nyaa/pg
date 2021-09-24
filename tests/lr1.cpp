#define YUKI_PG_PRINT
#include"lr1_gen.hpp"
int main(){
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
    yuki::pg::Token_Info t_info = {2,true,4};
    yuki::pg::Rule_Set<unsigned short> rs;
    rs.insert_equal({3,{3,4},1});
    rs.insert_equal({3,{4},2});

    rs.insert_equal({4,{0,4,1},3});
    rs.insert_equal({4,{0,1},4});

    yuki::pg::First_Table<unsigned short> ft(t_info,rs);

    yuki::pg::lr1::LR1_Item_Set<unsigned short> items;
    items.insert({3,{3,4},2,0});
    items.insert({3,{3,4},0,0});
    items.insert({3,{4},2,0});
    items.insert({3,{4},0,0});
    items.insert({4,{0,4,1},2,0});
    items.insert({4,{0,4,1},0,0});
    items.insert({4,{0,1},2,0});
    items.insert({4,{0,1},0,0});
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
    std::cout<<"items"<<'\n';
    for(auto i=items.begin();i!=items.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 8 8 3
    yuki::print_space(std::cout,items.size(),items.mid_size(),items.coarse_size());
    std::cout<<std::endl;

    yuki::pg::lr1::LR1_Item_Set<unsigned short> cc0 = yuki::pg::lr1::generate_initial_items(3,rs,ft);
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
    std::cout<<"cc0"<<'\n';
    for(auto i=cc0.begin();i!=cc0.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 8 8 3
    yuki::print_space(std::cout,cc0.size(),cc0.mid_size(),cc0.coarse_size());
    std::cout<<std::endl;
    yuki::pg::lr1::LR1_Item_Set<unsigned short> cc3 = yuki::pg::lr1::go_to(cc0,0,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),0,1)
    (4,(0,1,),2,1)
    (4,(0,4,1,),0,1)
    (4,(0,4,1,),2,1)
    */
   std::cout<<"cc3"<<'\n';
    for(auto i=cc3.begin();i!=cc3.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 6 6 3
    yuki::print_space(std::cout,cc3.size(),cc3.mid_size(),cc3.coarse_size());
    std::cout<<std::endl;
    yuki::pg::lr1::LR1_Item_Set<unsigned short> cc1 = yuki::pg::lr1::go_to(cc0,3,rs,ft);
    /*
    (4,(0,1,),0,0)
    (4,(0,1,),2,0)
    (4,(0,4,1,),0,0)
    (4,(0,4,1,),2,0)
    (3,(3,4,),0,1)
    (3,(3,4,),2,1)
    */
   std::cout<<"cc1"<<'\n';
    for(auto i=cc1.begin();i!=cc1.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 6 6 2
    yuki::print_space(std::cout,cc1.size(),cc1.mid_size(),cc1.coarse_size());
    std::cout<<std::endl;
    yuki::pg::lr1::LR1_Item_Set<unsigned short> cc6 = yuki::pg::lr1::go_to(cc3,0,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),1,1)
    (4,(0,4,1,),1,1)
    */
   std::cout<<"cc6"<<'\n';
    for(auto i=cc6.begin();i!=cc6.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 4 4 3
    yuki::print_space(std::cout,cc6.size(),cc6.mid_size(),cc6.coarse_size());
    std::cout<<std::endl;
    yuki::pg::lr1::LR1_Item_Set<unsigned short> cc6_2 = yuki::pg::lr1::go_to(cc6,0,rs,ft);
    /*
    (4,(0,1,),1,0)
    (4,(0,4,1,),1,0)
    (4,(0,1,),1,1)
    (4,(0,4,1,),1,1)
    */
   std::cout<<"cc6_2"<<'\n';
    for(auto i=cc6_2.begin();i!=cc6_2.end();++i){
        i.print(std::cout);
        std::cout<<'\n';
    }
    // 4 4 3
    yuki::print_space(std::cout,cc6_2.size(),cc6_2.mid_size(),cc6_2.coarse_size());
    std::cout<<std::endl;

    // 1 0 0 1
    yuki::print_space(std::cout,cc6==cc6_2,cc0==cc1,cc3==cc0,cc6==cc6);

    yuki::print_space(std::cout,sizeof(char),sizeof(yuki::pg::lr1::LR1_Item_Set<unsigned short>));
}