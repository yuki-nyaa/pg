// Definition for the parsing tables and the parsing function.
#include"cconfig"
#include<yuki/pg/lr1.hpp>
#include"parser.h"
constinit xxx::Parser::Action_Table xxx::Parser::action_table = {
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

constinit xxx::Parser::Goto_Table xxx::Parser::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
};

int xxx::Parser::parse(){
    namespace ypg = yuki::pg;

    reset();

    Any_Token token_ahead_ = lexer_->lex();

    while(true){
        switch(action_table(state_,token_ahead_.kind()).kind()){
            case ypg::LR1_Action_Kind::S : {
                state_ = action_table(state_,token_ahead_.kind()).state();
                stack_.push_back({std::move(token_ahead_),state_});
                token_ahead_ = lexer_->lex();
                goto loop_end_;
            }
            case ypg::LR1_Action_Kind::R : {
                switch(action_table(state_,token_ahead_.kind()).rule()){
                    case 0: {
                        assert(stack_.size()>=1);
                        size_t start_ = stack_.size()-1-(1-1);
                        size_t end_ = stack_.size()-1;
                        Token::List& token0_ = stack_[start_].token.get<Token::List>();

                        { // User codes.
                            printf("%d\n",token0_.zeroth);
                            printf("(");
                            for(const size_t e : token0_.first)
                                 printf("%d ",e);
                            printf(")\n");
                            fflush(stdout);
                        }

                        stack_[start_].token.static_destroy_deallocate<Token::List>();
                        stack_.pop_back();

                        token_ahead_.static_destroy_deallocate<Token::EOF_>();

                        return 0;
                    }
                    case 1 : {
                        assert(stack_.size()>=2);
                        size_t start_ = stack_.size()-1-(2-1);
                        size_t end_ = stack_.size()-1;
                        Token::List& token0_ = stack_[start_].token.get<Token::List>();
                        Token::Pair& token1_ = stack_[start_+1].token.get<Token::Pair>();

                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::List>();
                        YUKI_CONSTRUCT(p_token_target_,{token0_.zeroth+token1_.zeroth});

                        { // User codes.
                            p_token_target_->first.merge(token0_.first);
                            p_token_target_->first.merge(token1_.first);
                            p_token_target_->first.push_back(1);
                        }

                        stack_[start_+1].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        stack_[start_].token.static_destroy_deallocate<Token::List>();
                        stack_.pop_back();

                        if(!stack_.empty()){
                            state_ = goto_table(stack_.back().state,Token_Kind::List);
                        }else{
                            state_ = goto_table(0,Token_Kind::List);
                        }

                        stack_.push_back({Any_Token(p_token_target_),state_});

                        goto loop_end_;
                    }
                    case 2 : {
                        assert(stack_.size()>=1);
                        size_t start_ = stack_.size()-1-(1-1);
                        size_t end_ = stack_.size()-1;
                        auto& token0_ = stack_[start_].token.get<Token::Pair>();

                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::List>();
                        YUKI_CONSTRUCT(p_token_target_,{token0_.zeroth,std::move(token0_.first)});

                        { // User codes.
                            p_token_target_->first.push_back(2);
                        }

                        stack_[start_].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();

                        if(!stack_.empty()){
                            state_ = goto_table(stack_.back().state,Token_Kind::List);
                        }else{
                            state_ = goto_table(0,Token_Kind::List);
                        }

                        stack_.push_back({Any_Token(p_token_target_),state_});

                        goto loop_end_;
                    }
                    case 3 : {
                        assert(stack_.size()>=3);
                        size_t start_ = stack_.size()-1-(3-1);
                        size_t end_ = stack_.size()-1;
                        auto& token0_ = stack_[start_].token.get<Token::LB>();
                        auto& token1_ = stack_[start_+1].token.get<Token::Pair>();
                        auto& token2_ = stack_[start_+2].token.get<Token::RB>();

                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::Pair>();
                        YUKI_CONSTRUCT(p_token_target_,{token1_.zeroth,std::move(token1_.first)});

                        { // User codes.
                            p_token_target_->first.push_back(3);
                        }

                        stack_[start_+2].token.static_destroy_deallocate<Token::RB>();
                        stack_.pop_back();
                        stack_[start_+1].token.static_destroy_deallocate<Token::Pair>();
                        stack_.pop_back();
                        stack_[start_].token.static_destroy_deallocate<Token::LB>();
                        stack_.pop_back();

                        if(!stack_.empty()){
                            state_ = goto_table(stack_.back().state,Token_Kind::Pair);
                        }else{
                            state_ = goto_table(0,Token_Kind::Pair);
                        }

                        stack_.push_back({Any_Token(p_token_target_),state_});

                        goto loop_end_;
                    }
                    case 4 : {
                        assert(stack_.size()>=2);
                        size_t start_ = stack_.size()-1-(2-1);
                        size_t end_ = stack_.size()-1;
                        auto& token0_ = stack_[start_].token.get<Token::LB>();
                        auto& token1_ = stack_[start_+1].token.get<Token::RB>();

                        auto p_token_target_ = Any_Token::alloc.template allocate<Token::Pair>();
                        YUKI_CONSTRUCT(p_token_target_,{1,{4}});

                        { // User codes.
                        }

                        stack_[start_+1].token.static_destroy_deallocate<Token::RB>();
                        stack_.pop_back();
                        stack_[start_].token.static_destroy_deallocate<Token::LB>();
                        stack_.pop_back();

                        if(!stack_.empty()){
                            state_ = goto_table(stack_.back().state,Token_Kind::Pair);
                        }else{
                            state_ = goto_table(0,Token_Kind::Pair);
                        }

                        stack_.push_back({Any_Token(p_token_target_),state_});

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
