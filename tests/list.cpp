#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"list.h"
namespace xxx{
yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_){
using enum Token_Settings::Token_Kind::enum_t;
switch(p_.state()){
case 0: S0: switch(t_.kind()){
    case List: p_.SHIFT_(std::move(t_),1); t_=l_.lex(); goto S1;
    case Pair: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case "(": p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    default: return {(size_t)-1,(size_t)-1};
} // case 0
case 1: S1: switch(t_.kind()){
    case Pair: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case "(": p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case EOF_: p_.SET_STATE(0); return {EOF_,0};
    default: return {(size_t)-1,(size_t)-1};
} // case 1
case 2: S2: switch(t_.kind()){
    case "(": p_.SET_STATE(2); return {"(",2};
    case EOF_: p_.SET_STATE(2); return {EOF_,2};
    default: return {(size_t)-1,(size_t)-1};
} // case 2
case 3: S3: switch(t_.kind()){
    case Pair: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case "(": p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    case ")": p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    default: return {(size_t)-1,(size_t)-1};
} // case 3
case 4: S4: switch(t_.kind()){
    case "(": p_.SET_STATE(1); return {"(",1};
    case EOF_: p_.SET_STATE(1); return {EOF_,1};
    default: return {(size_t)-1,(size_t)-1};
} // case 4
case 5: S5: switch(t_.kind()){
    case ")": p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    default: return {(size_t)-1,(size_t)-1};
} // case 5
case 6: S6: switch(t_.kind()){
    case "(": p_.SET_STATE(4); return {"(",4};
    case ")": p_.SET_STATE(4); return {")",4};
    case EOF_: p_.SET_STATE(4); return {EOF_,4};
    default: return {(size_t)-1,(size_t)-1};
} // case 6
case 7: S7: switch(t_.kind()){
    case "(": p_.SET_STATE(3); return {"(",3};
    case ")": p_.SET_STATE(3); return {")",3};
    case EOF_: p_.SET_STATE(3); return {EOF_,3};
    default: return {(size_t)-1,(size_t)-1};
} // case 7
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
            case 1:{ // List -> List Pair
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                std::list<size_t>& token0_ = stack_[start_+0].token.get<1>();
                size_t& token1_ = stack_[start_+1].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::List>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                std::list<size_t>& token_target_ = token_target_complete_.get<1>();

                {
         token_target_.merge(token0_);
         token_target_.push_back(token1_);
         token_target_.push_back(2);
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE List -> List Pair\n");
                break;
            } // case 1 //List -> List Pair
            case 2:{ // List -> Pair
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                size_t& token0_ = stack_[start_+0].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::List>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
                std::list<size_t>& token_target_ = token_target_complete_.get<1>();

                {
         token_target_.push_back(token0_);
         token_target_.push_back(1);
                }

                stack_.pop_back(1);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE List -> Pair\n");
                break;
            } // case 2 //List -> Pair
            case 4:{ // Pair -> "(" ")"
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Pair>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},(stack_[start_+1].token.location_range()).begin.line);
                size_t& token_target_ = token_target_complete_.get<2>();

                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Pair -> \"(\" \")\"\n");
                break;
            } // case 4 //Pair -> "(" ")"
            case 3:{ // Pair -> "(" Pair ")"
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                size_t& token1_ = stack_[start_+1].token.get<2>();
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Pair>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},token1_+1);
                size_t& token_target_ = token_target_complete_.get<2>();

                {(token_target_complete_.location_range()).end.column;
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Pair -> \"(\" Pair \")\"\n");
                break;
            } // case 3 //Pair -> "(" Pair ")"
            case 0:{ // Goal_ -> List
                assert(stack_.size()>=1);
                const size_t start_ = stack_.size()-1;
                std::list<size_t>& token0_ = stack_[start_+0].token.get<1>();
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
} // namespace xxx

//blah blah blah

