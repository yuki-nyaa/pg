#pragma once
#include<cstdio>
#include"gen/parser.token.hpp"

namespace xxx{
struct Lexer{
    FILE* in;

    Token_Settings::Token_t lex() const {
        typedef xxx::Token_Settings::Token Token;
        typedef yuki::pg::Any_TToken<xxx::Token_Settings> Any_TToken;
        while(1){
            switch(const int c=fgetc(in); c){
                case EOF:{
                    auto p = Any_TToken::alloc.template allocate<Token::EOF_>();
                    YUKI_CONSTRUCT(p,{});
                    return Any_TToken(p);
                }
                case static_cast<unsigned char>('('):{
                    auto p = Any_TToken::alloc.template allocate<Token::LB>();
                    YUKI_CONSTRUCT(p,{});
                    return Any_TToken(p);
                }
                case static_cast<unsigned char>(')'):{
                    auto p = Any_TToken::alloc.template allocate<Token::RB>();
                    YUKI_CONSTRUCT(p,{});
                    return Any_TToken(p);
                }
            }
        }
    }
};
}