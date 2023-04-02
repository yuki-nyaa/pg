
#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"gen/parser.h"
namespace xxx{
constinit SParser_Tables::Action_Table SParser_Tables::action_table = {
    {0,2,{1,3}},
    {1,2,{1,3}},{1,4,{2,0}},
    {2,2,{2,2}},{2,4,{2,2}},
    {3,2,{1,6}},{3,3,{1,7}},
    {4,2,{2,1}},{4,4,{2,1}},
    {5,3,{1,8}},
    {6,2,{1,6}},{6,3,{1,10}},
    {7,2,{2,4}},{7,4,{2,4}},
    {8,2,{2,3}},{8,4,{2,3}},
    {9,3,{1,11}},
    {10,3,{2,4}},
    {11,3,{2,3}},
}; // constinit SParser_Tables::Action_Table SParser_Tables::action_table

constinit SParser_Tables::Goto_Table SParser_Tables::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
}; // constinit SParser_Tables::Goto_Table SParser_Tables::goto_table

int SParser::parse(Lexer& l_){
    namespace ypg = yuki::pg;

    reset();
    stack_.reserve(128);

    Token_t t_ = l_.lex();

    while(true){
        switch(action_table(state_,t_.kind()).kind()){
            case ypg::LR1_Action_Kind::S:{
                state_ = action_table(state_,t_.kind()).state();
                stack_.emplace_back(std::move(t_),state_);
                t_ = l_.lex();
                goto loop_end_;
            }
            case ypg::LR1_Action_Kind::R:{
                switch(action_table(state_,t_.kind()).rule()){
                    case 2:{ // List -> Pair
                        assert(stack_.size()>=1);
                        const size_t start_ = stack_.size()-1;
                        Token_t& token0_ = stack_[start_+0].token;
                        Token_t token_target_{Token_Kind::List,token0_.value,{token0_.location_range.begin,token0_.location_range.end}};

                        {
         token_target_.value+=2;
                        }

                        stack_.pop_back(1);
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                        stack_.emplace_back(std::move(token_target_),state_);
                        YUKI_PG_DBGO("REDUCE List -> Pair\n");
                        break;
                    } // case 2 //List -> Pair
                    case 1:{ // List -> List Pair
                        assert(stack_.size()>=2);
                        const size_t start_ = stack_.size()-2;
                        Token_t& token0_ = stack_[start_+0].token;
                        Token_t& token1_ = stack_[start_+1].token;
                        Token_t token_target_{Token_Kind::List,token0_.value+token1_.value,{token0_.location_range.begin,token1_.location_range.end}};

                        {
         token_target_.value+=1;
                        }

                        stack_.pop_back(2);
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                        stack_.emplace_back(std::move(token_target_),state_);
                        YUKI_PG_DBGO("REDUCE List -> List Pair\n");
                        break;
                    } // case 1 //List -> List Pair
                    case 4:{ // Pair -> "(" ")"
                        assert(stack_.size()>=2);
                        const size_t start_ = stack_.size()-2;
                        Token_t& token0_ = stack_[start_+0].token;
                        Token_t& token1_ = stack_[start_+1].token;
                        Token_t token_target_{Token_Kind::Pair,7,{token0_.location_range.begin,token1_.location_range.end}};

                        {
                        }

                        stack_.pop_back(2);
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                        stack_.emplace_back(std::move(token_target_),state_);
                        YUKI_PG_DBGO("REDUCE Pair -> \"(\" \")\"\n");
                        break;
                    } // case 4 //Pair -> "(" ")"
                    case 3:{ // Pair -> "(" Pair ")"
                        assert(stack_.size()>=3);
                        const size_t start_ = stack_.size()-3;
                        Token_t& token0_ = stack_[start_+0].token;
                        Token_t& token1_ = stack_[start_+1].token;
                        Token_t& token2_ = stack_[start_+2].token;
                        Token_t token_target_{Token_Kind::Pair,token1_.value,{token0_.location_range.begin,token2_.location_range.end}};

                        {
         token_target_.value+=3;
                        }

                        stack_.pop_back(3);
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                        stack_.emplace_back(std::move(token_target_),state_);
                        YUKI_PG_DBGO("REDUCE Pair -> \"(\" Pair \")\"\n");
                        break;
                    } // case 3 //Pair -> "(" Pair ")"
                    case 0:{ // Goal_ -> List
                        assert(stack_.size()>=1);
                        const size_t start_ = stack_.size()-1;
                        Token_t& token0_ = stack_[start_+0].token;
                        {
         fprintf(stderr,"%d\n",token0_.value);
                        }

                        stack_.pop_back(1);
                        return 0;
                    }
                    default:return 2;
                } // switch(action_table(state_,t_.kind()).rule())
            } // case ypg::LR1_Action_Kind::R
            case ypg::LR1_Action_Kind::ERR:{
                yuki::print_error(stderr,"Syntax Error!\n");
                reset();
                yuki::print_error(stderr,"Stack Clear!\n");
                return 1;
            }
        } // switch(action_table(state_,t_.kind()).kind)
        loop_end_:;
    } // while(true)
} // int SParser::parse(Lexer&)
} // namespace xxx

