
#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"gen/parser.h"
namespace xxx{
constinit Parser_Tables::Action_Table Parser_Tables::action_table = {
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
}; // constinit Parser_Tables::Action_Table Parser_Tables::action_table

constinit Parser_Tables::Goto_Table Parser_Tables::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
}; // constinit Parser_Tables::Goto_Table Parser_Tables::goto_table

int Parser::parse(Lexer& l_){
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
                        Token::Pair& token0_ = stack_[start_+0].token.get<Token::Pair>();
                        auto p_token_target_ = Token_t::alloc.template allocate<Token::List>();
                        yuki::pg::Location_Range loc_target_ = {stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end};
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{token0_.zeroth,std::move(token0_.first)});
                        Token::List& token_target_ = *p_token_target_;

                        {
         token_target_.first.push_back(2);
                        }

                        stack_[start_+0].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                        stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);
                        YUKI_PG_DBGO("REDUCE List -> Pair\n");
                        break;
                    } // case 2 //List -> Pair
                    case 1:{ // List -> List Pair
                        assert(stack_.size()>=2);
                        const size_t start_ = stack_.size()-2;
                        Token::List& token0_ = stack_[start_+0].token.get<Token::List>();
                        Token::Pair& token1_ = stack_[start_+1].token.get<Token::Pair>();
                        auto p_token_target_ = Token_t::alloc.template allocate<Token::List>();
                        yuki::pg::Location_Range loc_target_ = {stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end};
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{token0_.zeroth+token1_.zeroth});
                        Token::List& token_target_ = *p_token_target_;

                        {
         token_target_.first.merge(token0_.first);
         token_target_.first.merge(token1_.first);
         token_target_.first.push_back(1);
                        }

                        stack_[start_+1].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        stack_[start_+0].token.static_destroy_deallocate<Token::List>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                        stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);
                        YUKI_PG_DBGO("REDUCE List -> List Pair\n");
                        break;
                    } // case 1 //List -> List Pair
                    case 4:{ // Pair -> "(" ")"
                        assert(stack_.size()>=2);
                        const size_t start_ = stack_.size()-2;
                        Token::LB& token0_ = stack_[start_+0].token.get<Token::LB>();
                        Token::RB& token1_ = stack_[start_+1].token.get<Token::RB>();
                        auto p_token_target_ = Token_t::alloc.template allocate<Token::Pair>();
                        yuki::pg::Location_Range loc_target_ = {stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end};
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{1,{4}});
                        Token::Pair& token_target_ = *p_token_target_;

                        {
                        }

                        stack_[start_+1].token.static_destroy_deallocate<Token::RB>();
                        stack_.pop_back();
                        stack_[start_+0].token.static_destroy_deallocate<Token::LB>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                        stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);
                        YUKI_PG_DBGO("REDUCE Pair -> \"(\" \")\"\n");
                        break;
                    } // case 4 //Pair -> "(" ")"
                    case 3:{ // Pair -> "(" Pair ")"
                        assert(stack_.size()>=3);
                        const size_t start_ = stack_.size()-3;
                        Token::LB& token0_ = stack_[start_+0].token.get<Token::LB>();
                        Token::Pair& token1_ = stack_[start_+1].token.get<Token::Pair>();
                        Token::RB& token2_ = stack_[start_+2].token.get<Token::RB>();
                        auto p_token_target_ = Token_t::alloc.template allocate<Token::Pair>();
                        yuki::pg::Location_Range loc_target_ = {stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end};
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{token1_.zeroth,std::move(token1_.first)});
                        Token::Pair& token_target_ = *p_token_target_;

                        {
         token_target_.first.push_back(3);
                        }

                        stack_[start_+2].token.static_destroy_deallocate<Token::RB>();
                        stack_.pop_back();
                        stack_[start_+1].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        stack_[start_+0].token.static_destroy_deallocate<Token::LB>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                        stack_.emplace_back(Token_t(p_token_target_,loc_target_),state_);
                        YUKI_PG_DBGO("REDUCE Pair -> \"(\" Pair \")\"\n");
                        break;
                    } // case 3 //Pair -> "(" Pair ")"
                    case 0:{ // Goal_ -> List
                        assert(stack_.size()>=1);
                        const size_t start_ = stack_.size()-1;
                        Token::List& token0_ = stack_[start_+0].token.get<Token::List>();
                        {
        printf("%d\n",token0_.zeroth);
        printf("(");
        for(const size_t e : token0_.first)
            printf("%d ",e);
        printf(")\n");
        fflush(stdout);
                        }

                        stack_[start_+0].token.static_destroy_deallocate<Token::List>();
                        stack_.pop_back();
                        t_.static_destroy_deallocate<Token::EOF_>();
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
} // int Parser::parse(Lexer&)
} // namespace xxx

//blah blah blah

