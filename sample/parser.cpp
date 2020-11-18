// Definition for the parsing tables and the parsing function.
#include<yuki/ParserGen/lr1.hpp>
#include"parser.h"
constinit xxx::Parser::Action_Table_t xxx::Parser::action_table = {
    {0,0,{1,3}}, {0,1,{1,3}}, {0,2,{2,2}}, {0,3,{1,6}}, {0,4,{2,1}}, {0,6,{1,6}}, {0,7,{2,4}}, {0,8,{2,3}},
    {1,3,{1,7}}, {1,5,{1,8}}, {1,6,{1,10}}, {1,9,{1,11}}, {1,10,{2,4}}, {1,11,{2,3}},
    {2,1,{2,0}}, {2,2,{2,2}}, {2,4,{2,1}}, {2,7,{2,4}}, {2,8,{2,3}},
};

constinit xxx::Parser::Goto_Table_t xxx::Parser::goto_table = {
    {3,0,1},
    {4,0,2}, {4,1,4}, {4,3,5}, {4,6,9},
};

int xxx::Parser::parse(){
    typedef Token_Settings::any_token any_token;
    typedef Token_Settings::Terminal Terminal;
    typedef Token_Settings::NTerminal NTerminal;
    typedef Token_Settings::Token_Kind Token_Kind;
    namespace ypg = yuki::pg;

    initialize();

    any_token token_ahead_ = lexer_ptr->lex();

    while(true){
        switch(action_table(token_ahead_.get_kind(),state_).get_kind()){
            case ypg::lr1::Action_Kind::S : {
                state_ = action_table(token_ahead_.get_kind(),state_).get_state();
                vec_.push_back({std::move(token_ahead_),state_});
                token_ahead_ = lexer_ptr->lex();
                goto loop_end_;
            }
            case ypg::lr1::Action_Kind::R : {
                switch(action_table(token_ahead_.get_kind(),state_).get_rule()){
                    case 0: {
                        assert(vec_.size()>=1);
                        size_t start_ = vec_.size()-1-(1-1);
                        size_t end_ = vec_.size()-1;
                        auto& token0_ = vec_[start_].token.static_get<NTerminal::List>();

                        { // User codes.
                            std::cout<<token0_.s0<<'\n';
                            std::cout<<'(';
                            for(const size_t e : token0_.s1){
                                 std::cout<<e<<' ';
                            }
                            std::cout<<')'<<std::endl;
                        }

                        vec_[start_].token.static_free<NTerminal::List>();
                        vec_.pop_back();

                        token_ahead_.static_free<Terminal::EoF>();

                        return 0;
                    }
                    case 1 : {
                        assert(vec_.size()>=2);
                        size_t start_ = vec_.size()-1-(2-1);
                        size_t end_ = vec_.size()-1;
                        auto& token0_ = vec_[start_].token.static_get<NTerminal::List>();
                        auto& token1_ = vec_[start_+1].token.static_get<NTerminal::Pair>();

                        any_token any_token_target_ = Token_Settings::make_any_token<NTerminal::List>();
                        auto& token_target_ = any_token_target_.static_get<NTerminal::List>();

                        { // User codes.
                            token_target_.s0=token0_.s0+token1_.s0;
                            token_target_.s1.merge(token0_.s1);
                            token_target_.s1.merge(token1_.s1);
                            token_target_.s1.push_back(1);
                        }

                        vec_[start_+1].token.static_free<NTerminal::Pair>();
                        vec_.pop_back();
                        vec_[start_].token.static_free<NTerminal::List>();
                        vec_.pop_back();

                        if(!vec_.empty()){
                            state_ = goto_table(Token_Kind::List,vec_.back().state);
                        }else{
                            state_ = goto_table(Token_Kind::List,0);
                        }

                        vec_.push_back({std::move(any_token_target_),state_});

                        goto loop_end_;
                    }
                    case 2 : {
                        assert(vec_.size()>=1);
                        size_t start_ = vec_.size()-1-(1-1);
                        size_t end_ = vec_.size()-1;
                        auto& token0_ = vec_[start_].token.static_get<NTerminal::Pair>();

                        any_token any_token_target_ = Token_Settings::make_any_token<NTerminal::List>();
                        auto& token_target_ = any_token_target_.static_get<NTerminal::List>();

                        { // User codes.
                            token_target_.s0=token0_.s0;
                            token_target_.s1=std::move(token0_.s1);
                            token_target_.s1.push_back(2);
                        }

                        vec_[start_].token.static_free<NTerminal::Pair>();
                        vec_.pop_back();

                        if(!vec_.empty()){
                            state_ = goto_table(Token_Kind::List,vec_.back().state);
                        }else{
                            state_ = goto_table(Token_Kind::List,0);
                        }

                        vec_.push_back({std::move(any_token_target_),state_});

                        goto loop_end_;
                    }
                    case 3 : {
                        assert(vec_.size()>=3);
                        size_t start_ = vec_.size()-1-(3-1);
                        size_t end_ = vec_.size()-1;
                        auto& token0_ = vec_[start_].token.static_get<Terminal::LB>();
                        auto& token1_ = vec_[start_+1].token.static_get<NTerminal::Pair>();
                        auto& token2_ = vec_[start_+2].token.static_get<Terminal::RB>();

                        any_token any_token_target_ = Token_Settings::make_any_token<NTerminal::Pair>();
                        auto& token_target_ = any_token_target_.static_get<NTerminal::Pair>();

                        { // User codes.
                            token_target_.s0+=token1_.s0;
                            token_target_.s1=std::move(token1_.s1);
                            token_target_.s1.push_back(3);
                        }

                        vec_[start_+2].token.static_free<Terminal::RB>();
                        vec_.pop_back();
                        vec_[start_+1].token.static_free<NTerminal::Pair>();
                        vec_.pop_back();
                        vec_[start_].token.static_free<Terminal::LB>();
                        vec_.pop_back();

                        if(!vec_.empty()){
                            state_ = goto_table(Token_Kind::Pair,vec_.back().state);
                        }else{
                            state_ = goto_table(Token_Kind::Pair,0);
                        }

                        vec_.push_back({std::move(any_token_target_),state_});

                        goto loop_end_;
                    }
                    case 4 : {
                        assert(vec_.size()>=2);
                        size_t start_ = vec_.size()-1-(2-1);
                        size_t end_ = vec_.size()-1;
                        auto& token0_ = vec_[start_].token.static_get<Terminal::LB>();
                        auto& token1_ = vec_[start_+1].token.static_get<Terminal::RB>();

                        any_token any_token_target_ = Token_Settings::make_any_token<NTerminal::Pair>();
                        auto& token_target_ = any_token_target_.static_get<NTerminal::Pair>();

                        { // User codes.
                            ++token_target_.s0;
                            token_target_.s1.push_back(4);
                        }

                        vec_[start_+1].token.static_free<Terminal::RB>();
                        vec_.pop_back();
                        vec_[start_].token.static_free<Terminal::LB>();
                        vec_.pop_back();

                        if(!vec_.empty()){
                            state_ = goto_table(Token_Kind::Pair,vec_.back().state);
                        }else{
                            state_ = goto_table(Token_Kind::Pair,0);
                        }

                        vec_.push_back({std::move(any_token_target_),state_});

                        goto loop_end_;
                    }
                }
            }
            case ypg::lr1::Action_Kind::ERR : {
                std::cerr<<"Syntax Error!"<<std::endl;
                initialize();
                std::cerr<<"Stack Clear!"<<std::endl;
                return 1;
            }
        }
        loop_end_:;
    }
}
