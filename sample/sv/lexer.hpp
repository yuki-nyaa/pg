#pragma once
#include<cstdio>
#include"gen/parser.token.hpp"

namespace xxx{
struct Lexer{
    FILE* in;

    Token_Settings::Token_t lex() const {
        typedef Token_Settings::Token_Kind Token_Kind;
        typedef Token_Settings::Token_t Token_t;
        while(1){
            switch(const int c=fgetc(in); c){
                case EOF: return Token_t(yuki::pg::in_place_kind<Token_Kind::EOF_>,{});
                case static_cast<unsigned char>('('): return Token_t(yuki::pg::in_place_kind<Token_Kind::LB>,{});
                case static_cast<unsigned char>(')'): return Token_t(yuki::pg::in_place_kind<Token_Kind::RB>,{});
            }
        }
    }
};
}