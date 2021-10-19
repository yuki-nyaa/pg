%simple_token int
%nterm Pair{};
%nterm List{};
%term LB "(" {};
%term RB ")" {};

%prec "("
%left ")"

%code a{
     //a
}

%code b{
     //b
}

%%
List :
    List Pair    %prec "("   %rr 5
    {$0+$1}
    {
         $$+=1;
    }
    |
    Pair    %prec 100
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

Goal_ :
    List {} {
         fprintf(stderr,"%d\n",$0);
    };

%%
//blah blah blah