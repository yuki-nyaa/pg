%option fast unicode nodefault noline
%option outfile="lexer.cpp" header-file="lexer.h"
%option namespace="xxx" prefix="bracket" lexer="Lexer" lex="lex" token-type="xxx::Token_Settings::Token_t"

%code_htop{
    #include"parser.token.hpp"
}

%code_cpptop{
    #include"parser.token.hpp"
}

%code_class{
    public:
        typedef xxx::Token_Settings Token_Settings;
        typedef xxx::Token_Settings::Token_Kind Token_Kind;
        typedef xxx::Token_Settings::Token_t Token_t;
}

%%
<INITIAL>{
\(    {
    return Token_t{Token_Kind::LB,2};
}
\)    {
    return Token_t{Token_Kind::RB,3};
}
<<EOF>>    {
    return Token_t{Token_Kind::EOF_,5};
}
}
%%