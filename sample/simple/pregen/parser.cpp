// Definition for the parsing tables and the parsing function.
#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"parser.h"

constinit yuki::pg::LR1_Action_Table<xxx::Token_Settings,12,5> xxx::SParser::action_table = {
    {0,2,{1,3}},
    {1,2,{1,3}}, {1,4,{2,0}},
    {2,2,{2,2}}, {2,4,{2,2}},
    {3,2,{1,6}}, {3,3,{1,7}},
    {4,2,{2,1}}, {4,4,{2,1}},
    {5,3,{1,8}},
    {6,2,{1,6}}, {6,3,{1,10}},
    {7,2,{2,4}}, {7,4,{2,4}},
    {8,2,{2,3}}, {8,4,{2,3}},
    {9,3,{1,11}},
    {10,3,{2,4}},
    {11,3,{2,3}},
};

constinit yuki::pg::LR1_Goto_Table<xxx::Token_Settings,12> xxx::SParser::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
};

int xxx::SParser::parse(xxx::Lexer& lexer_p_){
    namespace ypg = yuki::pg;

    reset();
    stack_.reserve(128);

    Any_Token token_ahead_=lexer_p_.lex();

    while(true){
        switch(action_table(state_,token_ahead_.kind).kind()){
            case ypg::LR1_Action_Kind::S : {
                state_ = action_table(state_,token_ahead_.kind).state();
                stack_.emplace_back(std::move(token_ahead_),state_);
                token_ahead_=lexer_p_.lex();
                goto loop_end_;
            }
            case ypg::LR1_Action_Kind::R : {
                switch(action_table(state_,token_ahead_.kind).rule()){
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
                        goto loop_end_;
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
                        goto loop_end_;
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
                        goto loop_end_;
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
                        goto loop_end_;
                    }
                    default : return 2;
                }
            }
            case ypg::LR1_Action_Kind::ERR : {
                yuki::print_error(stderr,"Syntax Error!\n");
                reset();
                yuki::print_error(stderr,"Stack Clear!\n");
                return 1;
            }
        }
        loop_end_:;
    }
}
