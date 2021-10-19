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
        typedef xxx::Token_Settings::Token Token;
        typedef yuki::pg::Any_Token<xxx::Token_Settings> Any_Token;
}

%%
<INITIAL>{
\(    {
    auto p = Any_Token::alloc.template allocate<Token::LB>();
    YUKI_CONSTRUCT(p,{});
    return Any_Token(p);
    }
\)    {
    auto p = Any_Token::alloc.template allocate<Token::RB>();
    YUKI_CONSTRUCT(p,{});
    return Any_Token(p);
    }
<<EOF>>    {
    auto p = Any_Token::alloc.template allocate<Token::EOF_>();
    YUKI_CONSTRUCT(p,{});
    return Any_Token(p);
    }
}
%%