#include"cconfig"
#include"lexer.h"
#include"parser.h"
#include<cstdio>
typedef xxx::Token_Settings TS;
int main(){
    FILE* file=fopen("test.txt","r");
    xxx::Lexer lexer(file);
    xxx::SParser<1> parser1(&lexer);
    parser1.parse();
}