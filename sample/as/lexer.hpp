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
                case EOF: return Token_t{Token_Kind::EOF_,5};
                case static_cast<unsigned char>('('): return Token_t{Token_Kind::LB,2};
                case static_cast<unsigned char>(')'): return Token_t{Token_Kind::RB,3};
            }
        }
    }
};
}