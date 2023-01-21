#include"debug.hpp"
#include"lexer.h"
#include"parser.h"
#include<cstdio>
typedef xxx::Token_Settings TS;
int main(){
    FILE* file=fopen("test.txt","r");
    xxx::Lexer lexer(file);
    xxx::Parser parser(&lexer);
    parser.parse();
}
