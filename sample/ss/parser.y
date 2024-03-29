%simple_token int
%nterm List
%nterm Pair
%term LB "("
%term RB ")"

%namespace xxx
%parser SParser

%code cpp_top{
#include"cconfig"
}

%code h_top{
#include"lexer.hpp"
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
         $$+=($!$.begin.line+1);
    }
    |
    Pair
    {$0}
    {
         $$+=($!0.end.line+2);
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
