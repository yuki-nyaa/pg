#pragma once

#include"lexer.h"
#include<yuki/pg/lr1.hpp>
#include"parser.token.hpp"
namespace xxx{
struct Parser  : yuki::pg::AbsLR1Parser<xxx::Token_Settings>{
    xxx::Lexer* lexer;

    typedef yuki::pg::LR1_Action_Table<xxx::Token_Settings,12,5> Action_Table;
    typedef yuki::pg::LR1_Goto_Table<xxx::Token_Settings,12> Goto_Table;
    static constinit Action_Table action_table;
    static constinit Goto_Table goto_table;

    constexpr Parser(xxx::Lexer* l=nullptr) noexcept : lexer(l) {}

    int parse(xxx::Lexer&);

    virtual int parse() override  {return parse(*lexer);}


};
} // namespace xxx
