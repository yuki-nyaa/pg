#include"cconfig"
#include<yuki/print.hpp>
#include<yuki/pg/lr1.hpp>
#include"gen/parser.h"
namespace xxx{
yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_){
    using enum Token_Settings::Token_Kind::enum_t;
    switch(p_.state()){
    case 0: CC0: switch(t_.kind()){
        case LB: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto CC3;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 1: CC1: switch(t_.kind()){
        case LB: p_.SHIFT_(std::move(t_),3); t_=l_.lex(); goto CC3;
        case EOF_: p_.SET_STATE(1); return {EOF_,0};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 2: CC2: switch(t_.kind()){
        case LB: p_.SET_STATE(2); return {LB,2};
        case EOF_: p_.SET_STATE(2); return {EOF_,2};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 3: CC3: switch(t_.kind()){
        case LB: p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto CC6;
        case RB: p_.SHIFT_(std::move(t_),7); t_=l_.lex(); goto CC7;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 4: CC4: switch(t_.kind()){
        case LB: p_.SET_STATE(4); return {LB,1};
        case EOF_: p_.SET_STATE(4); return {EOF_,1};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 5: CC5: switch(t_.kind()){
        case RB: p_.SHIFT_(std::move(t_),8); t_=l_.lex(); goto CC8;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 6: CC6: switch(t_.kind()){
        case LB: p_.SHIFT_(std::move(t_),6); t_=l_.lex(); goto CC6;
        case RB: p_.SHIFT_(std::move(t_),10); t_=l_.lex(); goto CC10;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 7: CC7: switch(t_.kind()){
        case LB: p_.SET_STATE(7); return {LB,4};
        case EOF_: p_.SET_STATE(7); return {EOF_,4};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 8: CC8: switch(t_.kind()){
        case LB: p_.SET_STATE(8); return {LB,3};
        case EOF_: p_.SET_STATE(8); return {EOF_,3};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 9: CC9: switch(t_.kind()){
        case RB: p_.SHIFT_(std::move(t_),11); t_=l_.lex(); goto CC11;
        default : return {(size_t)-1,(size_t)-1};
    }
    case 10: CC10: switch(t_.kind()){
        case RB: p_.SET_STATE(10); return {RB,4};
        default : return {(size_t)-1,(size_t)-1};
    }
    case 11: CC11: switch(t_.kind()){
        case RB: p_.SET_STATE(11); return {RB,3};
        default : return {(size_t)-1,(size_t)-1};
    }
    default: return {(size_t)-1,(size_t)-1};
    } // switch(p_.state())
} // yuki::pg::lr1_action_return_type Parser_Tables::Action_Table::operator()(yuki::pg::AbsLR1Parser<Token_Settings>& p_,Lexer& l_)

constinit Parser_Tables::Goto_Table Parser_Tables::goto_table = {
    {0,0,1},{0,1,2},
    {1,1,4},
    {3,1,5},
    {6,1,9},
}; // constinit Parser_Tables::Goto_Table Parser_Tables::goto_table

int Parser::parse(Lexer& l_){
    reset();
    stack_.reserve(128);
    Action_Table action_table(l_);

    while(true){
        yuki::pg::lr1_action_return_type art_ = action_table(*this,l_);
        switch(art_.rule){
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
                YUKI_CONSTRUCT_BRACE(p_token_target_,{(stack_[start_+1].token.location_range()).begin.line+1,{4}});
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
         token_target_.first.push_back(loc_target_.end.column+3);
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
                action_table.static_destroy_deallocate<Token::EOF_>();
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

