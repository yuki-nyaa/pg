%nterm Regex {FSM}
%nterm Char_Class {yuki::IntegralCIs_OV<char32_t>}
%nterm Char_Interval {yuki::CInterval<char32_t>}
%nterm Char_Intervals {yuki::IntegralCIs_OV<char32_t>}
%term amount {Amount}
%term character {char32_t}
%term meta_char {int}
%term basic_char_class {yuki::IntegralCIs_OV<char32_t>}
%term paren_l "("
%term paren_r ")"
%term sqparen_l "["
%term sqparen_r "]"
%term ast "*"
%term plus "+"
%term qmark "?"
%term qe "?="
%term lazy "?l:"
%term dlazy "?d:"
%term vbar "|"
%term hat "^"
%term to "-"
%term esc_N

%default_left
%right amount "*" "+" "?"

%namespace yuki::lex
%parser Regex_Parser
%token_settings Regex_Parser_TS
%lexer Regex_Lexer

%no_default_ctor

%code cpp_top{
#include"cconfig"
}

%code h_top{
#include<yuki/Vector.hpp>
#include"Regex_Lexer.hpp"
#ifndef YUKI_LEX_FSM_FACTORY_POOL_BLOCK
#define YUKI_LEX_FSM_FACTORY_POOL_BLOCK 256
#endif
#ifndef YUKI_LEX_FSMS_RESERVE
#define YUKI_LEX_FSMS_RESERVE 64
#endif
}

%code token_top{
#include"FSM.hpp"
}

%code class{
  private:
    FSM_Factory<Node_Pool<FSM_Node,YUKI_LEX_FSM_FACTORY_POOL_BLOCK>> ff;
    size_t branch=0;
    unsigned& errors;

    yuki::Vector<FSM> fsms_{yuki::reserve_tag,YUKI_LEX_FSMS_RESERVE};
    size_t max_branch_size_ = 0;
    size_t max_branch_number_ = 0;
    yuki::Vector<size_t> heads_;
  public:
    void recycle(){
        ff.recycle();
        branch=0;
        fsms_.clear();
        max_branch_size_=0;
        max_branch_number_=0;
        heads_.clear();
    }

    explicit Regex_Parser(Regex_Lexer* const l,unsigned& e) noexcept : lexer(l), errors(e) {}

    size_t total_branches() const {return branch;}
    const FSM* fsms() const {return fsms_.begin();}
    size_t fsms_size() const {return fsms_.size();}
    size_t max_branch_size() const {return max_branch_size_;}
    size_t max_branch_number() const {return max_branch_number_;}
    const auto& heads() const {return heads_;}
}

%%
Goal_
    Regex
        {}
        {
            FSM fsm = $0;
            fsm.make_accept();
            if(fsm.size>max_branch_size_)
                max_branch_size_ = fsm.size;
            if(fsm.accept->number > max_branch_number_)
                max_branch_number_ = fsm.accept->number;
            fsms_.emplace_back(fsm);
            ++branch;
            ff.reset_number();
        }
    ;

Regex
    Regex Regex
        {ff.make_seq($0,$1)}
        {}
    |
    Regex "|" Regex
        {ff.make_alt(branch,$0,$2)}
        {}
    |
    "(" Regex ")"
        {$1}
        {}
    |
    Regex "*"
        {ff.make_ast(branch,$0)}
        {}
    |
    Regex "+"
        {ff.make_plus(branch,$0)}
        {}
    |
    Regex "?"
        {ff.make_qmark(branch,$0)}
        {}
    |
    Regex amount
        {ff.make_amount(branch,$0,$1)}
        {}
    |
    "(" "?=" Regex ")"
        {$2.make_head()}
        {
            if(!heads_.empty() && heads_.back()==branch){
                fprintf(stderr,"Error: Multiple lookaheads in branch %zu!\n",branch);
                ++errors;
            }else
                heads_.emplace_back(branch);
        }
    |
    "(" "?l:" Regex ")"
        {$2.make_lazy()}
        {}
    |
    "(" "?d:" Regex ")"
        {$2.make_dlazy()}
        {}
    |
    Char_Class
        {ff.make_fsm(branch,std::move($0))}
        {}
    |
    character
        {ff.make_fsm(branch,yuki::CInterval<char32_t>{$0,$0})}
        {}
    |
    meta_char
        {ff.make_fsm(branch,$0)}
        {}
    |
    "^"
        {ff.make_fsm(branch,MetaChar::BOL)}
        {}
    |
    esc_N
        {ff.make_esc_N(branch)}
        {}
    ;

//Alts
//    Alts "|" Regex
//        {($0.emplace_back($2),std::move($0))}
//        {}
//    |
//    Regex "|" Regex
//        {yuki::from_variadic_tag,$0,$2}
//        {}
//    ;

Char_Class
    "[" Char_Interval "]"
        {$1}
        {}
    |
    "[" Char_Intervals "]"
        {std::move($1)}
        {}
    |
    "[" Char_Class "+" Char_Class "]"
        {$1+$3}
        {}
    |
    "[" Char_Class "*" Char_Class "]"
        {$1*$3}
        {}
    |
    "[" "^" Char_Class "]"
        {yuki::negate<{U'\0',yuki::UNICODE_MAX_32}>($2)}
        {}
    |
    "[" "^" Char_Intervals "]"
        {yuki::negate<{U'\0',yuki::UNICODE_MAX_32}>($2)}
        {}
    |
    "[" Char_Class "-" Char_Class "]"
        {$1-$3}
        {}
    |
    basic_char_class
        {std::move($0)}
        {}
    ;

Char_Intervals
    Char_Intervals Char_Interval
    {std::move(($0.insert($1),$0))}
    {}
    |
    Char_Interval
    {$0}
    {}
    ;

Char_Interval
    character "-" character
    {$0,$2}
    {}
    |
    character
    {$0,$0}
    {}
    ;
%%
