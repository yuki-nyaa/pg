#include"cconfig"


#include<yuki/print.hpp>
#include"list.h"

namespace xxx{
size_t Parser_term_action(
    const size_t s_,
    yuki::pg::stack_t<yuki::pg::Token_State_Pair<Token_Settings::Token_t>>& stack_,
    Token_Settings::Token_t& t_
    Lexer& l_,
    #ifdef YUKI_PG_Parser_DBG
    ,FILE* const fp_dbg_
    #endif
){
using enum Token_Settings::Token_Kind::enum_t;
auto shift_ = [](Token_Settings::Token_t&& tp,const size_t sp){
};
switch(s_){
case 0: switch(t_.kind()){
    case "(": shift_(std::move(t_),3); goto S3;
    default: return (size_t)-1;
} // case 0
S1: t_=l_.lex(); case 1: switch(t_.kind()){
    case "(": shift_(std::move(t_),3); goto S3;
    case EOF_: return 0;
    default: return (size_t)-1;
} // S1
S2: t_=l_.lex(); case 2: switch(t_.kind()){
    case "(": return 2;
    case EOF_: return 2;
    default: return (size_t)-1;
} // S2
S3: t_=l_.lex(); case 3: switch(t_.kind()){
    case "(": shift_(std::move(t_),3); goto S3;
    case ")": shift_(std::move(t_),6); goto S6;
    default: return (size_t)-1;
} // S3
S4: t_=l_.lex(); case 4: switch(t_.kind()){
    case "(": return 1;
    case EOF_: return 1;
    default: return (size_t)-1;
} // S4
S5: t_=l_.lex(); case 5: switch(t_.kind()){
    case ")": shift_(std::move(t_),7); goto S7;
    default: return (size_t)-1;
} // S5
S6: t_=l_.lex(); case 6: switch(t_.kind()){
    case "(": return 4;
    case ")": return 4;
    case EOF_: return 4;
    default: return (size_t)-1;
} // S6
S7: t_=l_.lex(); case 7: switch(t_.kind()){
    case "(": return 3;
    case ")": return 3;
    case EOF_: return 3;
    default: return (size_t)-1;
} // S7
default: return (size_t)-1;
} // switch(s_)
} // Parser_term_action


size_t Parser_nterm_transition(const size_t s_,const unsigned long long nt_){
using enum Token_Settings::Token_Kind::enum_t;
switch(s_){
case 0: switch(nt_){
    case List: return 1;
    case Pair: return 2;
    default: return -1;
} // case 0
case 1: switch(nt_){
    case Pair: return 4;
    default: return -1;
} // case 1
case 3: switch(nt_){
    case Pair: return 5;
    default: return -1;
} // case 3
default: return -1;
} // switch(s_)
} // Parser_nterm_transition


template<typename L_>
int Parser::parse(L_& l_){
assert(stack_.empty());
stack_.reserve(128);
size_t s_=0;
typedef Token_Settings::Token_t Token_t;
Token_t t_ = l_.lex();

while(1){
switch(Parser_term_action(s_,stack_,t_,l_
#ifdef YUKI_PG_Parser_DBG
,fp_dbg_
#endif
)){
case 1:{ // List -> List Pair
    assert(stack_.size()>=2);
    const size_t start_ = stack_.size()-2;
    std::list<size_t >& token0_ = stack_[start_+0].token.get<1>();
    size_t& token1_ = stack_[start_+1].token.get<2>();
    Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::List>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end});
    std::list<size_t >& token_target_ = token_target_complete_.get<1>();

    {

        /*target*/.merge( /*rights[0]*/);
        fputs("\"}",stdout);
        fputc(static_cast<unsigned char>('}'),stdout);
        /*target*/.push_back(/*rights[1]*/);
        /*target*/.push_back(2);
    
    }

    stack_.pop_back(2);
    s_ = Parser_nterm_transition(stack_.empty()?0:stack_.back().state, Token_Kind::List);
    stack_.emplace_back(std::move(token_target_complete_),s_);
    YUKI_PG_DBGO("REDUCE List -> List Pair\n");
    break;
} // case 1 // List -> List Pair
case 2:{ // List -> Pair
    assert(stack_.size()>=1);
    const size_t start_ = stack_.size()-1;
    size_t& token0_ = stack_[start_+0].token.get<2>();
    Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::List>,{stack_[start_].token.location_range().begin,stack_[start_+0].token.location_range().end});
    std::list<size_t >& token_target_ = token_target_complete_.get<1>();

    {
/*target*/.push_back(/*rights[0]*/);
        /*target*/.push_back(1);
    
    }

    stack_.pop_back(1);
    s_ = Parser_nterm_transition(stack_.empty()?0:stack_.back().state, Token_Kind::List);
    stack_.emplace_back(std::move(token_target_complete_),s_);
    YUKI_PG_DBGO("REDUCE List -> Pair\n");
    break;
} // case 2 // List -> Pair
case 4:{ // Pair -> "(" ")"
    assert(stack_.size()>=2);
    const size_t start_ = stack_.size()-2;
    Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Pair>,{stack_[start_].token.location_range().begin,stack_[start_+1].token.location_range().end},$!1.begin.line);
    size_t& token_target_ = token_target_complete_.get<2>();

    {

    }

    stack_.pop_back(2);
    s_ = Parser_nterm_transition(stack_.empty()?0:stack_.back().state, Token_Kind::Pair);
    stack_.emplace_back(std::move(token_target_complete_),s_);
    YUKI_PG_DBGO("REDUCE Pair -> \"(\" \")\"\n");
    break;
} // case 4 // Pair -> "(" ")"
case 3:{ // Pair -> "(" Pair ")"
    assert(stack_.size()>=3);
    const size_t start_ = stack_.size()-3;
    size_t& token1_ = stack_[start_+1].token.get<2>();
    Token_t token_target_complete_(yuki::pg::in_place_kind<Token_Kind::Pair>,{stack_[start_].token.location_range().begin,stack_[start_+2].token.location_range().end},/*rights[1]*/+1);
    size_t& token_target_ = token_target_complete_.get<2>();

    {
$!$.end.column;
    }

    stack_.pop_back(3);
    s_ = Parser_nterm_transition(stack_.empty()?0:stack_.back().state, Token_Kind::Pair);
    stack_.emplace_back(std::move(token_target_complete_),s_);
    YUKI_PG_DBGO("REDUCE Pair -> \"(\" Pair \")\"\n");
    break;
} // case 3 // Pair -> "(" Pair ")"
case 0:{ // Goal_ -> List
    assert(stack_.size()>=1);
    const size_t start_ = stack_.size()-1;
    std::list<size_t >& token0_ = stack_[start_+0].token.get<1>();
    {
printf("(");
        for(const size_t e : /*rights[0]*/)
            printf("%d ",e);
        printf(")\n");
        fflush(stdout);
    
    }

    stack_.pop_back(1);
    return 0;
    }
default:{
    fputs("Syntax Error!\n",stderr);
    reset();
    fputs("Stack Clear!\n",stderr);
    return 1;
}
} // switch(Parser_term_action(...))
} // while(1)
} // int Parser::parse(L_&)

template int Parser::parse<Lexer>(Lexer&);

} // namespace xxx


//blah blah blah

#include"lexer.hpp"




