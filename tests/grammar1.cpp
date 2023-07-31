
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"grammar1.h"
yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_){
using enum Token_Settings::Token_Kind::enum_t;
switch(p_.state()){
case 0: S0: switch(t_.kind()){
    case X: p_.SHIFT_(std::move(t_),1); t_=l_.lex(); goto S1;
    case Y: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case a: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case c: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case EOF_: p_.SET_STATE(4); return {EOF_,4};
    default: return {(size_t)-1,(size_t)-1};
} // case 0
case 1: S1: if(t_.kind()==EOF_){p_.SET_STATE(0); return {EOF_,0};} else return {(size_t)-1,(size_t)-1};
case 2: S2: switch(t_.kind()){
    case EOF_: p_.SET_STATE(1); return {EOF_,1};
    default: return {(size_t)-1,(size_t)-1};
} // case 2
case 3: S3: switch(t_.kind()){
    case EOF_: p_.SET_STATE(2); return {EOF_,2};
    default: return {(size_t)-1,(size_t)-1};
} // case 3
case 4: S4: switch(t_.kind()){
    case EOF_: p_.SET_STATE(3); return {EOF_,3};
    default: return {(size_t)-1,(size_t)-1};
} // case 4
default: return {(size_t)-1,(size_t)-1};
} // switch(p_.state())
} // yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_)

constinit Parser_Tables::Goto_Table Parser_Tables::goto_table = {
}; // constinit Parser_Tables::Goto_Table Parser_Tables::goto_table

int Parser::parse(Lexer& l_){
    reset();
    stack_.reserve(128);
    Action_Table action_table(l_);

    while(true){
        yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);
        switch(art_.rule){
            case 1:{ // X -> Y
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> Y\n");
                break;
            } // case 1 //X -> Y
            case 2:{ // X -> a
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> a\n");
                break;
            } // case 2 //X -> a
            case 3:{ // Y -> c
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Y>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Y);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Y -> c\n");
                break;
            } // case 3 //Y -> c
            case 4:{ // Y ->
                assert(stack_.size()>=0);
                const size_t start_ = stack_.size()-0;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Y>,{});
                {
                }

                stack_.pop_back(0);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Y);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Y ->\n");
                break;
            } // case 4 //Y ->
            case 5:{ // Z -> X Y Z
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Z>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Z);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Z -> X Y Z\n");
                break;
            } // case 5 //Z -> X Y Z
            case 6:{ // Z -> d
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Z>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
                {
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Z);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Z -> d\n");
                break;
            } // case 6 //Z -> d
            case 0:{ // Goal_ -> X
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                {
                }

                stack_.pop_back(1);
                return 0;
            }
            default:{
                yuki::print_error(stderr,"Syntax Error!\n");
                reset();
                yuki::print_error(stderr,"Stack Clear!\n");
                return 1;
            }
        } // switch(art_.rule)
    } // while(true)
} // int Parser::parse(Lexer&)

//blah blah blah

