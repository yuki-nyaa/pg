#pragma once
#include<yuki/Vector.hpp>
#include"Regex_Lexer.hpp"
#ifndef YUKI_LEX_FSM_FACTORY_POOL_BLOCK
#define YUKI_LEX_FSM_FACTORY_POOL_BLOCK 256
#endif
#ifndef YUKI_LEX_FSMS_RESERVE
#define YUKI_LEX_FSMS_RESERVE 64
#endif
#include<yuki/pg/lr1.hpp>
#include"Regex_Parser.token.hpp"
namespace yuki::lex{
struct Regex_Parser_Tables{
    struct Action_Table{
      private:
        Regex_Parser_TS::Token_t t_;
      public:
        Action_Table(Regex_Lexer& l_) noexcept : t_(l_.lex()) {}
        yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<Regex_Parser_TS>&,Regex_Lexer&);
    };
    typedef yuki::pg::LR1_Goto_Table<Regex_Parser_TS,52> Goto_Table;
    static constinit Goto_Table goto_table;
};

struct Regex_Parser final : yuki::pg::AbsLR1Parser<Regex_Parser_TS>, private Regex_Parser_Tables {
    Regex_Lexer* lexer;

    using Regex_Parser_Tables::Action_Table;
    using Regex_Parser_Tables::Goto_Table;
    using Regex_Parser_Tables::goto_table;

    int parse(Regex_Lexer&);

    virtual int parse() override final {assert(lexer); return parse(*lexer);}

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

}; // struct Regex_Parser
} // namespace yuki::lex

