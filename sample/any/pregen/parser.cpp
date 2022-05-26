#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"parser.h"
constinit xxx::Parser::Action_Table xxx::Parser::action_table = {
    {0,2,{1,3}},
    {1,2,{1,3}},
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
    {1,4,{2,0}},
};

constinit xxx::Parser::Goto_Table xxx::Parser::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},

};

int xxx::Parser::parse(xxx::Lexer& lexer_p_){
    namespace ypg = yuki::pg;

    reset();
    stack_.reserve(128);

    Any_Token token_ahead_ = lexer_p_.lex();

    while(true){
        switch(action_table(state_,token_ahead_.kind()).kind()){
            case ypg::LR1_Action_Kind::S : {
                state_ = action_table(state_,token_ahead_.kind()).state();
                stack_.emplace_back(std::move(token_ahead_),state_);
                token_ahead_ = lexer_p_.lex();
                goto loop_end_;
            }
            case ypg::LR1_Action_Kind::R : {
                switch(action_table(state_,token_ahead_.kind()).rule()){
                    case 2 : { // List -> Pair
                        assert(stack_.size()>=1);
                        size_t start_ = stack_.size()-1;
                        Token::Pair& token0_ = stack_[start_+0].token.get<Token::Pair>();
                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::List>();
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{token0_.zeroth,std::move(token0_.first)});
                        Token::List& token_target_ = *p_token_target_;

                        {
         token_target_.first.push_back(2);
                        }

                        stack_[start_+0].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::List);
                        stack_.emplace_back(Any_Token(p_token_target_),state_);
                        goto loop_end_;
                    }
                    case 1 : { // List -> List Pair
                        assert(stack_.size()>=2);
                        size_t start_ = stack_.size()-2;
                        Token::List& token0_ = stack_[start_+0].token.get<Token::List>();
                        Token::Pair& token1_ = stack_[start_+1].token.get<Token::Pair>();
                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::List>();
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
                        stack_.emplace_back(Any_Token(p_token_target_),state_);
                        goto loop_end_;
                    }
                    case 4 : { // Pair -> "(" ")"
                        assert(stack_.size()>=2);
                        size_t start_ = stack_.size()-2;
                        Token::LB& token0_ = stack_[start_+0].token.get<Token::LB>();
                        Token::RB& token1_ = stack_[start_+1].token.get<Token::RB>();
                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::Pair>();
                        YUKI_CONSTRUCT_BRACE(p_token_target_,{1,{4}});
                        Token::Pair& token_target_ = *p_token_target_;

                        {
                        }

                        stack_[start_+1].token.static_destroy_deallocate<Token::RB>();
                        stack_.pop_back();
                        stack_[start_+0].token.static_destroy_deallocate<Token::LB>();
                        stack_.pop_back();
                        state_ = goto_table(stack_.empty() ? 0 : stack_.back().state,Token_Kind::Pair);
                        stack_.emplace_back(Any_Token(p_token_target_),state_);
                        goto loop_end_;
                    }
                    case 3 : { // Pair -> "(" Pair ")"
                        assert(stack_.size()>=3);
                        size_t start_ = stack_.size()-3;
                        Token::LB& token0_ = stack_[start_+0].token.get<Token::LB>();
                        Token::Pair& token1_ = stack_[start_+1].token.get<Token::Pair>();
                        Token::RB& token2_ = stack_[start_+2].token.get<Token::RB>();
                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::Pair>();
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
                        stack_.emplace_back(Any_Token(p_token_target_),state_);
                        goto loop_end_;
                    }
                    case 0 : { // Goal_ -> List
                        assert(stack_.size()>=1);
                        size_t start_ = stack_.size()-1;
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
                        token_ahead_.static_destroy_deallocate<Token::EOF_>();
                        return 0;
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

//blah blah blah
