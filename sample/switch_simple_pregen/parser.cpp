// Definition for the parsing tables and the parsing function.
#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"parser.h"

namespace xxx{
yuki::pg::lr1_action_return_type SParser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_){
    using enum Token_Settings::Token_Kind::enum_t;
    switch(p_.state()){
    case 0: CC0: switch(t_.kind){
        case LB: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto CC3;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 1: CC1: switch(t_.kind){
        case LB: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto CC3;
        case EOF_: p_.SET_STATE(1); return {EOF_,0};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 2: CC2: switch(t_.kind){
        case LB: p_.SET_STATE(2); return {LB,2};
        case EOF_: p_.SET_STATE(2); return {EOF_,2};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 3: CC3: switch(t_.kind){
        case LB: p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto CC6;
        case RB: p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto CC7;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 4: CC4: switch(t_.kind){
        case LB: p_.SET_STATE(4); return {LB,1};
        case EOF_: p_.SET_STATE(4); return {EOF_,1};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 5: CC5: switch(t_.kind){
        case RB: p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto CC8;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 6: CC6: switch(t_.kind){
        case LB: p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto CC6;
        case RB: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto CC10;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 7: CC7: switch(t_.kind){
        case LB: p_.SET_STATE(7); return {LB,4};
        case EOF_: p_.SET_STATE(7); return {EOF_,4};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 8: CC8: switch(t_.kind){
        case LB: p_.SET_STATE(8); return {LB,3};
        case EOF_: p_.SET_STATE(8); return {EOF_,3};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 9: CC9: switch(t_.kind){
        case RB: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto CC11;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 10: CC10: switch(t_.kind){
        case RB: p_.SET_STATE(10); return {RB,4};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 11: CC11: switch(t_.kind){
        case RB: p_.SET_STATE(11); return {RB,3};
        default : return {(size_t)-1,(size_t)-1};
    }
    default : return {(size_t)-1,(size_t)-1};
    } // switch(p_.state())
} // yuki::pg::lr1_action_return_type SParser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<xxx::Token_Settings>& p_,Lexer& l_)


constinit yuki::pg::LR1_Goto_Table<Token_Settings,12> SParser_Tables::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
}; // constinit yuki::pg::LR1_Goto_Table<Token_Settings,12> SParser::goto_table

int SParser::parse(Lexer& l_){
    reset();
    stack_.reserve(128);
    Action_Table action_table(l_);
    while(true){
        yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);
        switch(art_.rule){
            case 0: {
                assert(stack_.size()>=1);
                size_t start_ = stack_.size()-1-(1-1);
                Any_Token& token0_ = stack_[start_].token;

                { // User codes.
                    fprintf(stderr,"%d\n",token0_.value);
                }

                stack_.pop_back();

                return 0;
            }
            case 1 : {
                assert(stack_.size()>=2);
                size_t start_ = stack_.size()-1-(2-1);
                Any_Token& token0_ = stack_[start_].token;
                Any_Token& token1_ = stack_[start_+1].token;
                Any_Token token_target_{Token_Kind::List,token0_.value+token1_.value};

                { // User codes.
                    token_target_.value+=1;
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                stack_.emplace_back(std::move(token_target_),state_);
                break;
            }
            case 2 : {
                assert(stack_.size()>=1);
                size_t start_ = stack_.size()-1-(1-1);
                Any_Token& token0_ = stack_[start_].token;
                Any_Token token_target_{Token_Kind::List,token0_.value};

                { // User codes.
                    token_target_.value+=2;
                }

                stack_.pop_back();
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                stack_.emplace_back(std::move(token_target_),state_);
                break;
            }
            case 3 : {
                assert(stack_.size()>=3);
                size_t start_ = stack_.size()-1-(3-1);
                Any_Token& token0_ = stack_[start_].token;
                Any_Token& token1_ = stack_[start_+1].token;
                Any_Token& token2_ = stack_[start_+2].token;
                Any_Token token_target_{Token_Kind::Pair,token1_.value};

                { // User codes.
                    token_target_.value+=3;
                }

                stack_.pop_back(3);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                stack_.emplace_back(std::move(token_target_),state_);
                break;
            }
            case 4 : {
                assert(stack_.size()>=2);
                size_t start_ = stack_.size()-1-(2-1);
                Any_Token& token0_ = stack_[start_].token;
                Any_Token& token1_ = stack_[start_+1].token;
                Any_Token token_target_{Token_Kind::Pair,7};

                { // User codes.
                }

                stack_.pop_back(2);
                state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                stack_.emplace_back(std::move(token_target_),state_);
                break;
            }
            default : {
                yuki::print_error(stderr,"Syntax Error!\n");
                reset();
                yuki::print_error(stderr,"Stack Clear!\n");
                return 1;
            }
        } // switch(art_.rule)
    } // while(true)
} // int SParser::parse(xxx::Lexer& l_)
} // namespace xxx