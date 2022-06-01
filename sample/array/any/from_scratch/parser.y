%nterm List{
     size_t : count;
     std::list<size_t> : lst;
};
%nterm Pair{
     size_t : count;
     std::list<size_t> : lst;
} 4 ;
%term LB "(" {} yuki::Allocator<Token::LB> ;
%term RB ")" {};

%namespace xxx

%code token_top{
#include<list>
}

%code h_top{
#include"lexer.h"
}

%action_array

%%
List :
    List Pair    %prec "("   %rr 5
    {$0[count]+$1[count]}
    {
         $$[lst].merge($0[lst]);
         $$[lst].merge($1[lst]);
         $$[lst].push_back(1);
    }
    |
    Pair    %prec 100
    {$0[count],std::move($0[lst])}
    {
         $$[lst].push_back(2);
    };


Pair :
    "(" Pair ")"
    {$1[count],std::move($1[lst])}
    {
         $$[lst].push_back(3);
    }
    |
    "(" ")"
    {1,{4}}
    {};

Goal_ :
    List {} {
        printf("%d\n",$0[count]);
        printf("(");
        for(const size_t e : $0[lst])
            printf("%d ",e);
        printf(")\n");
        fflush(stdout);
    };

%%
//blah blah blah