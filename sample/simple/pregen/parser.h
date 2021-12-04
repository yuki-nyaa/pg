// LR1-parser class definition and parsing table declarations.
#pragma once
#include<yuki/pg/lr1.hpp>
#include"parser.token.hpp"
#include"lexer.h"
namespace xxx{
struct SParser : yuki::pg::AbsLR1Parser<Token_Settings,Lexer,12,5>{
    typedef yuki::pg::AbsLR1Parser<Token_Settings,Lexer,12,5> Base_Parser;

    static constinit Action_Table action_table;
    static constinit Goto_Table goto_table;

    using Base_Parser::Base_Parser;

    virtual int parse() override;
};
} // namespace xxx
