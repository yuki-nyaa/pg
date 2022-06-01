// LR1-parser class definition and parsing table declarations.
#pragma once
#include<yuki/pg/lr1.hpp>
#include"parser.token.hpp"
#include"lexer.h"

namespace xxx{
struct SParser_Tables{
    struct Action_Table{
      private:
        yuki::pg::Any_Token<Token_Settings> t_;
      public:
        Action_Table(Lexer& l_) noexcept : t_(l_.lex()) {}
        yuki::pg::lr1_action_return_type operator()(yuki::pg::AbsLR1Parser<Token_Settings>&,Lexer&);
    };
    typedef yuki::pg::LR1_Goto_Table<Token_Settings,12> Goto_Table;
    static constinit Goto_Table goto_table;
};

struct SParser : yuki::pg::AbsLR1Parser<Token_Settings>, private SParser_Tables {
    Lexer* lexer;

    using SParser_Tables::Action_Table;
    using SParser_Tables::Goto_Table;
    using SParser_Tables::goto_table;

    constexpr SParser(Lexer* l=nullptr) noexcept : lexer(l) {}

    int parse(Lexer&);

    virtual int parse() override {return parse(*lexer);}
};
} // namespace xxx
