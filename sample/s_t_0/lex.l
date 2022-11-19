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
        typedef xxx::Token_Settings::Token Token;
        typedef yuki::pg::Any_TToken<xxx::Token_Settings> Any_TToken;
}

%%
<INITIAL>{
\(    {
    auto p = Any_TToken::alloc.template allocate<Token::LB>();
    YUKI_CONSTRUCT(p,{});
    return Any_TToken(p);
    }
\)    {
    auto p = Any_TToken::alloc.template allocate<Token::RB>();
    YUKI_CONSTRUCT(p,{});
    return Any_TToken(p);
    }
<<EOF>>    {
    auto p = Any_TToken::alloc.template allocate<Token::EOF_>();
    YUKI_CONSTRUCT(p,{});
    return Any_TToken(p);
    }
}
%%
