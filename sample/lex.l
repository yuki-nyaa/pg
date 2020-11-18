%option fast unicode nodefault noline
%option outfile="lexer.cpp" header-file="lexer.h"
%option namespace="xxx" prefix="bracket" lexer="Lexer" lex="lex" token-type="xxx::Token_Settings::any_token"

%code_htop{
    #include"parser.common.hpp"
}

%code_cpptop{
    #include"parser.common.hpp"
}

%code_class{
    public:
        typedef xxx::Token_Settings Token_Settings;
}

%%
<INITIAL>{
\(    {return Token_Settings::make_any_token<Token_Settings::Terminal::LB>();}
\)    {return Token_Settings::make_any_token<Token_Settings::Terminal::RB>();}
<<EOF>>    {return Token_Settings::make_any_token<Token_Settings::Terminal::EoF>();}
}
%%