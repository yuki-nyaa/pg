%nterm List {std::list<size_t/**/>}
%nterm Pair size_t
%terms LB"("RB/*blah*/")"
%term foo "\"" {decltype('\'')}
%term foo2 "\'" {decltype('\"')}

%namespace xxx

%code cpp_top{
#include"cconfig"
}

%code token_top{
#include<list>
}

%code h_top{
#include"lexer.hpp"
}

%%
List
    List Pair    %s "("   %r 5 %empty
    {}
    {
        // }
        $$.merge(/*}*/$0);
        fputs("\"}",stdout);
        fputc(static_cast<unsigned char>('}'),stdout);
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


Pair
    "(" Pair ")"
    {$1+1}
    {$!$.end.column;}
    |
    "(" ")"
    {$!1.begin.line}
    {};

Goal_
    List {/**/} {
        printf("(");
        for(const size_t e : $0)
            printf("%d ",e);
        printf(")\n");
        fflush(stdout);
    };

%%
//blah blah blah
