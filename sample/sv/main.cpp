#include"cconfig"
#include"lexer.hpp"
#include"gen/parser.h"

int main(){
    xxx::Lexer lexer(fopen("test.txt","r"));
    xxx::Parser parser(&lexer);
    parser.parse();
}
