#include"cconfig"
#include"lexer.hpp"
#include"gen/parser.h"
#include<cstdio>
int main(){
    xxx::Lexer lexer(fopen("test.txt","r"));
    xxx::Parser parser(&lexer);
    parser.parse();
}
