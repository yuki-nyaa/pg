// LR1-parser class definition and parsing table declarations.
#pragma once
#include<yuki/pg/lr1.hpp>
#include"parser.token.hpp"
#include"lexer.h"
namespace xxx{
struct SParser_Tables{
    typedef yuki::pg::LR1_Action_Table<Token_Settings,12,5> Action_Table;
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,12> Goto_Table;

    static constinit Action_Table action_table;
    static constinit Goto_Table goto_table;
};

template<int I>
struct SParser : yuki::pg::AbsLR1Parser<Token_Settings>,SParser_Tables{
    xxx::Lexer* lexer;

    constexpr SParser(xxx::Lexer* l=nullptr) noexcept : lexer(l) {}

    int parse(xxx::Lexer&);

    virtual int parse() override {return parse(*lexer);}
};
} // namespace xxx
