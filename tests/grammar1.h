#pragma once

#include<yuki/pg/lr1.hpp>
#include"grammar1.token.hpp"
struct Parser_Tables{
    struct Action_Table{
      private:
        Token_Settings::Token_t t_;
      public:
        Action_Table(Lexer& l_) noexcept : t_(l_.lex()) {}
        yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<Token_Settings>&,Lexer&);
    };
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,5> Goto_Table;
    static constinit Goto_Table goto_table;
};

struct Parser final : yuki::pg::AbsLR1Parser<Token_Settings>, private Parser_Tables {
    Lexer* lexer;

    using Parser_Tables::Action_Table;
    using Parser_Tables::Goto_Table;
    using Parser_Tables::goto_table;

    constexpr Parser() noexcept = default;
    explicit constexpr Parser(Lexer* const l) noexcept : lexer(l) {}

    int parse(Lexer&);

    virtual int parse() override final {assert(lexer); return parse(*lexer);}



}; // struct Parser

