%option fast unicode nodefault noline
%option outfile="lexer.cpp" header-file="lexer.h"
%option namespace="xxx" prefix="bracket" lexer="Lexer" lex="lex" token-type="yuki::pg::Any_Token<xxx::Token_Settings>"

%code_htop{
    #include"parser.common.hpp"
}

%code_cpptop{
    #include"parser.common.hpp"
}

%code_class{
    public:
        typedef xxx::Token_Settings Token_Settings;
        typedef xxx::Token_Settings::Token_Kind Token_Kind;
        typedef xxx::Token_Settings::Token Token;
        typedef yuki::pg::Any_Token<Token_Settings> Any_Token;
}

%%
<INITIAL>{
\(    {
    return Any_Token{Token_Kind::LB,2};
}
\)    {
    return Any_Token{Token_Kind::RB,3};
}
<<EOF>>    {
    return Any_Token{Token_Kind::EOF_,5};
}
}
%%