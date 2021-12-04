%simple_token int
%nterm List{};
%nterm Pair{};
%term LB "(" {};
%term RB ")" {};

%namespace xxx
%parser SParser

%code h_top{
#include"lexer.h"
}

%%
Goal_ :
    List {} {
         fprintf(stderr,"%d\n",$0);
    };

List :
    List Pair
    {$0+$1}
    {
         $$+=1;
    }
    |
    Pair
    {$0}
    {
         $$+=2;
    };

Pair :
    "(" Pair ")"
    {$1}
    {
         $$+=3;
    }
    |
    "(" ")"
    {7}
    {};
%%
