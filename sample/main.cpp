#include"lexer.h"
#include"parser.h"
#include<iostream>
#include<fstream>
typedef xxx::Token_Settings TS;
int main(){
    std::ifstream file("test.txt");
    xxx::Lexer lexer(file);
    xxx::Parser parser(&lexer);
    parser.parse();
}