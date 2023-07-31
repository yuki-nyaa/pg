
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"grammar2.h"
yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_){
using enum Token_Settings::Token_Kind::enum_t;
switch(p_.state()){
case 0: S0: switch(t_.kind()){
    case X: p_.SHIFT_(std::move(t_),1); t_=l_.lex(); goto S1;
    case a: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case b: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    default: return {(size_t)-1,(size_t)-1};
} // case 0
case 1: S1: if(t_.kind()==EOF_){p_.SET_STATE(0); return {EOF_,0};} else return {(size_t)-1,(size_t)-1};
case 2: S2: switch(t_.kind()){
    case Y: p_.SHIFT_(std::move(t_),4); t_=l_.lex(); goto S4;
    case Z: p_.SHIFT_(std::move(t_),5); t_=l_.lex(); goto S5;
    case T: p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto S6;
    case t: p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case u: p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    default: return {(size_t)-1,(size_t)-1};
} // case 2
case 3: S3: switch(t_.kind()){
    case Y: p_.SHIFT_(std::move(t_),9); t_=l_.lex(); goto S9;
    case Z: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto S10;
    case T: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto S11;
    case t: p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto S7;
    case u: p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto S8;
    default: return {(size_t)-1,(size_t)-1};
} // case 3
case 4: S4: switch(t_.kind()){
    case d: p_.SHIFT_(std::move(t_),12); t_=l_.lex(); goto S12;
    default: return {(size_t)-1,(size_t)-1};
} // case 4
case 5: S5: switch(t_.kind()){
    case c: p_.SHIFT_(std::move(t_),13); t_=l_.lex(); goto S13;
    default: return {(size_t)-1,(size_t)-1};
} // case 5
case 6: S6: switch(t_.kind()){
    case a: p_.SET_STATE(3); return {a,3};
    case d: p_.SET_STATE(3); return {d,3};
    case e: p_.SET_STATE(3); return {e,3};
    case EOF_: p_.SET_STATE(3); return {EOF_,3};
    default: return {(size_t)-1,(size_t)-1};
} // case 6
case 7: S7: switch(t_.kind()){
    case W: p_.SHIFT_(std::move(t_),14); t_=l_.lex(); goto S14;
    case u: p_.SHIFT_(std::move(t_),15); t_=l_.lex(); goto S15;
    default: return {(size_t)-1,(size_t)-1};
} // case 7
case 8: S8: switch(t_.kind()){
    case X: p_.SHIFT_(std::move(t_),16); t_=l_.lex(); goto S16;
    case a: p_.SHIFT_(std::move(t_),2); t_=l_.lex(); goto S2;
    case b: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto S3;
    default: return {(size_t)-1,(size_t)-1};
} // case 8
case 9: S9: switch(t_.kind()){
    case e: p_.SHIFT_(std::move(t_),17); t_=l_.lex(); goto S17;
    default: return {(size_t)-1,(size_t)-1};
} // case 9
case 10: S10: switch(t_.kind()){
    case d: p_.SHIFT_(std::move(t_),18); t_=l_.lex(); goto S18;
    default: return {(size_t)-1,(size_t)-1};
} // case 10
case 11: S11: switch(t_.kind()){
    case a: p_.SET_STATE(6); return {a,6};
    case d: p_.SET_STATE(6); return {d,6};
    case e: p_.SET_STATE(6); return {e,6};
    case EOF_: p_.SET_STATE(6); return {EOF_,6};
    default: return {(size_t)-1,(size_t)-1};
} // case 11
case 12: S12: switch(t_.kind()){
    case a: p_.SET_STATE(1); return {a,1};
    case d: p_.SET_STATE(1); return {d,1};
    case e: p_.SET_STATE(1); return {e,1};
    case EOF_: p_.SET_STATE(1); return {EOF_,1};
    default: return {(size_t)-1,(size_t)-1};
} // case 12
case 13: S13: switch(t_.kind()){
    case a: p_.SET_STATE(2); return {a,2};
    case d: p_.SET_STATE(2); return {d,2};
    case e: p_.SET_STATE(2); return {e,2};
    case EOF_: p_.SET_STATE(2); return {EOF_,2};
    default: return {(size_t)-1,(size_t)-1};
} // case 13
case 14: S14: switch(t_.kind()){
    case d: p_.SET_STATE(7); return {d,7};
    case e: p_.SET_STATE(7); return {e,7};
    default: return {(size_t)-1,(size_t)-1};
} // case 14
case 15: S15: switch(t_.kind()){
    case V: p_.SHIFT_(std::move(t_),19); t_=l_.lex(); goto S19;
    case c: p_.SET_STATE(9); return {c,9};
    case d: p_.SET_STATE(9); return {d,9};
    case e: p_.SET_STATE(12); return {e,12};
    default: return {(size_t)-1,(size_t)-1};
} // case 15
case 16: S16: switch(t_.kind()){
    case a: p_.SHIFT_(std::move(t_),20); t_=l_.lex(); goto S20;
    case d: p_.SET_STATE(8); return {d,8};
    case e: p_.SET_STATE(8); return {e,8};
    default: return {(size_t)-1,(size_t)-1};
} // case 16
case 17: S17: switch(t_.kind()){
    case a: p_.SET_STATE(4); return {a,4};
    case d: p_.SET_STATE(4); return {d,4};
    case e: p_.SET_STATE(4); return {e,4};
    case EOF_: p_.SET_STATE(4); return {EOF_,4};
    default: return {(size_t)-1,(size_t)-1};
} // case 17
case 18: S18: switch(t_.kind()){
    case a: p_.SET_STATE(5); return {a,5};
    case d: p_.SET_STATE(5); return {d,5};
    case e: p_.SET_STATE(5); return {e,5};
    case EOF_: p_.SET_STATE(5); return {EOF_,5};
    default: return {(size_t)-1,(size_t)-1};
} // case 18
case 19: S19: switch(t_.kind()){
    case d: p_.SET_STATE(11); return {d,11};
    case e: p_.SET_STATE(11); return {e,11};
    default: return {(size_t)-1,(size_t)-1};
} // case 19
case 20: S20: switch(t_.kind()){
    case a: p_.SET_STATE(10); return {a,10};
    case d: p_.SET_STATE(10); return {d,10};
    case e: p_.SET_STATE(10); return {e,10};
    case EOF_: p_.SET_STATE(10); return {EOF_,10};
    default: return {(size_t)-1,(size_t)-1};
} // case 20
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
            case 3:{ // X -> a T
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> a T\n");
                break;
            } // case 3 //X -> a T
            case 1:{ // X -> a Y d
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> a Y d\n");
                break;
            } // case 1 //X -> a Y d
            case 2:{ // X -> a Z c
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> a Z c\n");
                break;
            } // case 2 //X -> a Z c
            case 6:{ // X -> b T
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> b T\n");
                break;
            } // case 6 //X -> b T
            case 4:{ // X -> b Y e
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> b Y e\n");
                break;
            } // case 4 //X -> b Y e
            case 5:{ // X -> b Z d
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::X>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::X);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE X -> b Z d\n");
                break;
            } // case 5 //X -> b Z d
            case 7:{ // Y -> t W
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Y>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Y);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Y -> t W\n");
                break;
            } // case 7 //Y -> t W
            case 8:{ // Y -> u X
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Y>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Y);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Y -> u X\n");
                break;
            } // case 8 //Y -> u X
            case 9:{ // Z -> t u
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Z>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Z);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE Z -> t u\n");
                break;
            } // case 9 //Z -> t u
            case 10:{ // T -> u X a
                assert(stack_.size()>=3);
                const size_t start_ = stack_.size()-3;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::T>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end});
                {
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::T);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE T -> u X a\n");
                break;
            } // case 10 //T -> u X a
            case 11:{ // W -> u V
                assert(stack_.size()>=2);
                const size_t start_ = stack_.size()-2;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::W>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
                {
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::W);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE W -> u V\n");
                break;
            } // case 11 //W -> u V
            case 12:{ // V ->
                assert(stack_.size()>=0);
                const size_t start_ = stack_.size()-0;
                Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::V>,{});
                {
                }

                stack_.pop_back(0);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::V);
                stack_.emplace_back(std::move(token_target_complete_),state_);
                YUKI_PG_DBGO("REDUCE V ->\n");
                break;
            } // case 12 //V ->
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

