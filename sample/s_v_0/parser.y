%nterm List {std::list<size_t>:;};
%nterm Pair {size_t:;};
%term LB "(" {};
%term RB ")" {};

%namespace xxx

%code token_top{
#include<list>
}

%code h_top{
#include"lexer.h"
}

%%
List :
    List Pair    %prec "("   %rr 5
    {}
    {
         $$.merge($0);
         $$.push_back($1);
         $$.push_back(2);
    }
    |
    Pair    %prec 100
    {}
    {
         $$.push_back($0);
         $$.push_back(1);
    };


Pair :
    "(" Pair ")"
    {$1+1}
    {$!$.end.column;}
    |
    "(" ")"
    {$!1.begin.line}
    {};

Goal_ :
    List {} {
        printf("(");
        for(const size_t e : $0)
            printf("%d ",e);
        printf(")\n");
        fflush(stdout);
    };

%%
//blah blah blah