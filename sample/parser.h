// LR1-parser class definition and parsing table declarations.
#pragma once
#include<yuki/ParserGen/lr1.hpp>
#include"parser.common.hpp"
#include"lexer.h"
namespace xxx{
struct Parser : private yuki::pg::lr1::AbsParser<Token_Settings,Lexer,11,4>{
  public:
    typedef yuki::pg::lr1::AbsParser<Token_Settings,Lexer,11,4> Base_;
    using typename Base_::Token_Settings;
    using typename Base_::Lexer_t;
    using typename Base_::state_t;
    using typename Base_::rule_num_t;
    using Base_::STATE_MAX;
    using Base_::STATE_INITIAL;
    using Base_::RULE_NUM_MAX;
    using typename Base_::Action_Table_t;
    using typename Base_::Goto_Table_t;
    using typename Base_::Token_State_Pair;
    using typename Base_::Any_Token;
    using typename Base_::Token;
    using typename Base_::Token_Kind;
    using Base_::stack_t;

    using Base_::Base_;

    using Base_::state;
    using Base_::stack;
    using Base_::lexer;
    using Base_::reset;

    static constinit Action_Table_t action_table;
    static constinit Goto_Table_t goto_table;

    int parse();
};
} // namespace xxx
